/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#define _DEBUG_
#include <pthread.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <cf_std.h>
#include <cf_common.h>
#include <cf_json.h>
#include <cf_cmd.h>
#include <netinet/in.h>      
#include <netdb.h>      
#include <stdio.h> 
#include <string.h>  
#include "medusa.h"
#include "mds_log.h"
#include "mds_msg_json_cmd.h"


#define MDS_RECO_EMAIL_PLUGIN_NAME       "PLUG_ECO_EMAIL"
#define MDS_ECO_EMAIL_ELEM_CLASS_NAME   "ECO_EMAIL"
#define EHLO "EHLO 163.com\r\n"      
#define DATA "data\r\n"      
#define QUIT "QUIT\r\n"

typedef struct mds_eco_email_elem {
    MDS_ELEM_MEMBERS;
	char *host_id="smtp.163.com";     
	char *from_id;     
	char *to_id;     
	char *sub="MonitorMail\r\n";     
	char wkstr[100]="this is a monitor mail\r\n";
	char *name_base64;
	char *password_base64;
	int sock;     
	struct sockaddr_in server;     
	struct hostent *hp, *gethostbyname();     
	char buf[BUFSIZ+1];     
	int len; 
}MdsEmailElem;


static int EmailPlugInit(MDSPlugin* this, MDSServer* svr);
static int EmailPlugExit(MDSPlugin* this, MDSServer* svr);

static MDSElem* EmailRequested(MDSServer* svr, CFJson* jConf);
static int EmailReleased(MDSElem* elem);
static int EmailProcess(MDSElem* self, MDSElem* vendor, MdsMsg* msg);
static int EmailAddedAsGuest(MDSElem* this, MDSElem* vendorElem);
static int EmailAddedAsVendor(MDSElem* this, MDSElem* guestElem);
static int EmailRemoveAsGuest(MDSElem* this, MDSElem* vendorElem);
static int EmailRemoveAsVendor(MDSElem* this, MDSElem* guestElem);

MdsElemClass EmailClass = {
    .name = MDS_ECO_EMAIL_ELEM_CLASS_NAME,
    .request = EmailRequested,
    .release = EmailReleased,
    .process = EmailProcess,
    .addedAsGuest = EmailAddedAsGuest,
    .addedAsVendor = EmailAddedAsVendor,
    .removeAsGuest = EmailRemoveAsGuest,
    .removeAsVendor = EmailRemoveAsVendor
};


MDS_PLUGIN("ecovacs_platform",
        MDS_RECO_EMAIL_PLUGIN_NAME,
        "plugin eco_email",
        EmailPlugInit,
        EmailPlugExit,
        0, 0, 1);

static int EmailAddedAsGuest(MDSElem* this, MDSElem* vendorElem)
{
    return 0;
}

static int EmailAddedAsVendor(MDSElem* this, MDSElem* guestElem)
{
    return 0;
}

static int EmailRemoveAsGuest(MDSElem* this, MDSElem* vendorElem)
{
    return 0;
}

static int EmailRemoveAsVendor(MDSElem* this, MDSElem* guestElem)
{
    return 0;
}
/*=====Send a string to the socket=====*/      
static void send_socket(MdsRKIspElem *elem,char *s)     
{         
	write(elem->sock,s,strlen(s));         
	//write(1,s,strlen(s));         
	//printf("Client:%s\n",s);     
}      
 
//=====Read a string from the socket=====*/      
static void read_socket(MdsRKIspElem *elem)     
{
	len = read(elem->sock,elem->buf,elem->BUFSIZ);
	write(1,elem->buf,elem->len);
	printf("Server:%s\n",elem->buf);     
}   
static int EmailSend(MdsRKIspElem *elem){
		/*=====Create Socket=====*/
	elem->sock = socket(AF_INET, SOCK_STREAM, 0);     
	if (elem->sock==-1)     
	{
		MDS_ERR_OUT(ERR_OUT, "ECO_EMAIL Create Socket failed\n");
	}     
	else 
		MDS_DBG("%s: todo : ECO_EMAIL socket created\n",__func__,tmpCStr); 

	/*=====Verify host=====*/
	elem->server.sin_family = AF_INET;
	elem->hp = gethostbyname(elem->host_id);
	if (elem->hp==(struct hostent *) 0)
	{
		MDS_ERR_OUT(ERR_OUT, "%s: unknown host\n", host_id)); 
	}
	printf("----------------------\n"); 
	/*=====Connect to port 25 on remote host=====*/
	memcpy((char *) &elem->server.sin_addr, (char *)elem->hp->h_addr, elem->hp->h_length);     	 				
	elem->server.sin_port=htons(25); /* SMTP PORT */
	if (connect(elem->sock, (struct sockaddr *) &elem->server, sizeof(elem->server))==-1)
	{
		MDS_ERR_OUT(ERR_OUT, "ECO_EMAIL connecting stream socket\n");
	}     
	else       
		//cout << "Connected\n";
		printf("Connected\n");
		
	read_socket(elem); /* SMTP Server logon string */
	send_socket(elem,EHLO); /* introduce ourselves */
	read_socket(elem); /*Read reply */
	/*
	**added for user email
	*/
	send_socket(elem,"AUTH LOGIN");
	send_socket(elem,"\r\n");
	read_socket(elem);
	send_socket(elem,"MTg1NTAyMDAzMDQ=");//用户名的base64编码
	send_socket(elem,"\r\n");
	read_socket(elem);
	send_socket(elem,"MXFhejJ3c3g=");//密码的base64编码
	send_socket(elem,"\r\n");
	read_socket(elem);
	/*send mail info */
	send_socket(elem,"mail from <");
	send_socket(elem,from_id);
	send_socket(elem,">");
	send_socket(elem,"\r\n");
	read_socket(elem); /* Sender OK */
 
	/*received mail info */
	send_socket(elem,"rcpt to <"); /*Mail to*/
	send_socket(elem,to_id);
	send_socket(elem,">");
	send_socket(elem,"\r\n");
	read_socket(elem); // Recipient OK*/
	
	/*msg info */
	send_socket(elem,DATA);// body to follow*/
	read_socket(elem); 
	
	send_socket(elem,"subject:");
	send_socket(elem,sub);

	send_socket(elem,"from:18550200304@163.com");

	send_socket(elem,"\r\n\r\n");
	send_socket(elem,wkstr);
	/*send msg */
	send_socket(elem,".\r\n");
	read_socket(elem);
	send_socket(elem,QUIT); /* quit */
	read_socket(elem); // log off */
		
	//=====Close socket and finish=====*/
	close(elem->sock);
	
	return 0;
	
ERR_OUT:
	return -1;
}

static int EmailExit(MdsRKIspElem *elem){

	return 0;
}

static int EmailStop(MdsRKIspElem *elem){

	return 0;
}

static int EmailStart(MdsRKIspElem *elem,int fd){

	return ret;
}

/*
 * mdsctl send_email '{"todo":"semd_email"}'
 */
static int EmailProcess(MDSElem* self, MDSElem* vendor, MdsMsg* msg)
{
    MdsRKIspElem *elem = (MdsRKIspElem*) self;

	if (!strcmp(MdsMsgGetType(msg), MDS_MSG_TYPE_JSON_CMD)) {
        MdsJsonCmdMsg *jcm;
        const char* tmpCStr;
        CFJson *jMsg, *jResp, *tmpJson;

        jcm = MdsMsgGetData(msg);
        jMsg = jcm->cmd;
        jResp = jcm->resp;

        if ((tmpCStr = CFJsonObjectGetString(jMsg, "todo"))) {
			MDS_DBG("%s: todo : %s +++++++\n",__func__,tmpCStr);
            if (!strcmp(tmpCStr, "send_email")) {
				if(EmailSend(elem)==0){
					CFJsonObjectAddString(jResp, "ret", "ok");
				}else{
					CFJsonObjectAddString(jResp, "ret", "fail");
				}
            } 
			MDS_DBG("%s: todo : %s ---------\n",__func__,tmpCStr);           
        } else {
            MDS_ERR_OUT(ERR_OUT, "unkonw request:%s\n", tmpCStr);
        }
    }
    return 0;

ERR_OUT:
    return -1;
}

static MDSElem* EmailRequested(MDSServer* svr, CFJson* jConf)
{
    MdsEmailElem *elem = NULL;
    const char *name = NULL;
    int ret = -1;
   	const char* tmpStr;

    if (!svr || !jConf) {
        MDS_ERR_OUT(ERR_OUT, "param error\n");
    }

    if (!(name = CFJsonObjectGetString(jConf, "name"))) {
        MDS_ERR_OUT(ERR_OUT, "CFJsonObjectGetString(name) failed\n");
    }

    if (!(elem = (MdsEmailElem*)calloc(1, sizeof(MdsEmailElem)))) {
        MDS_ERR_OUT(ERR_OUT, "calloc for MdsEmailElem failed\n");
    }
	
	if (!(tmpStr=CFJsonObjectGetString(jConf, "from_id"))) {
		MDS_ERR_OUT(ERR_EVTS_DEL, "get from_id from config failed\n");
	}
	strncpy(elem->from_id, tmpCStr, sizeof(elem->from_id));
	MDS_DBG("from_id: %s\n", elem->from_id);
	
	if (!(tmpStr=CFJsonObjectGetString(jConf, "to_id"))) {
		MDS_ERR_OUT(ERR_EVTS_DEL, "get to_id from config failed\n");
	}
	strncpy(elem->to_id, tmpCStr, sizeof(elem->to_id));
	MDS_DBG("to_id: %s\n", elem->to_id);
	
	if (!(tmpStr=CFJsonObjectGetString(jConf, "name_base64"))) {
		MDS_ERR_OUT(ERR_EVTS_DEL, "get name_base64 from config failed\n");
	}
	strncpy(elem->name_base64, tmpCStr, sizeof(elem->name_base64));
	MDS_DBG("name_base64: %s\n", elem->name_base64);
	
	if (!(tmpStr=CFJsonObjectGetString(jConf, "password_base64"))) {
		MDS_ERR_OUT(ERR_EVTS_DEL, "get password_base64 from config failed\n");
	}
	strncpy(elem->password_base64, tmpCStr, sizeof(elem->password_base64));
	MDS_DBG("password_base64: %s\n", elem->password_base64);
	
    if (MDSElemInit((MDSElem*) elem, svr,
                    &EmailClass, name, EmailProcess,
                    EmailAddedAsGuest, EmailAddedAsVendor,
                    EmailRemoveAsGuest, EmailRemoveAsVendor)) {
        MDS_ERR_OUT(ERR_EVTS_DEL, "MDSElem init failed: for %s\n", name);
    }

    return (MDSElem*) elem;


ERR_EVTS_DEL:
    free(elem);
    elem = NULL;
ERR_OUT:
    return NULL;
}

static int RKIspReleased(MDSElem* elem)
{
    assert(elem);

    MDSElemExit(elem);
    free(elem);
    MDS_DBG("\n");
    return 0;
}

static int EmailPlugInit(MDSPlugin* plg, MDSServer* svr)
{
    MDS_MSG("Initiating "MDS_RECO_EMAIL_PLUGIN_NAME"\n");
    return MDSServerRegistElemClass(svr, &EmailClass);
}

static int EmailPlugExit(MDSPlugin* plg, MDSServer* svr)
{
    MDS_MSG("Exiting "MDS_RECO_EMAIL_PLUGIN_NAME"\n");
    return MDSServerAbolishElemClass(svr, &EmailClass);
}
