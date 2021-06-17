#define index_page ""\
"<html>\n"\
    "<head>\n"\
	    "<title>LED Switch</title>\n"\
		"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>\n"\
        "<script>\n"\
        "function load(element,endPoint) { \n"\
           "var xhttp = new XMLHttpRequest(); \n"\
           "xhttp.onreadystatechange = function() {\n"\
               "if (this.readyState == 4 && this.status == 200) {\n"\
               "document.getElementById(element).innerHTML = this.responseText; \n"\
               "} \n"\
           "} \n"\
        "xhttp.open('GET', endPoint, true);\n"\
        "xhttp.send(); \n"\
        "} \n"\
		"function init(){ \n"\
		    "if (typeof lock == 'undefined'){load('data','led1.cgi');lock=1;}\n"\
			"if (typeof lock == 'undefined'){load('data2','led2.cgi');lock=1;}\n"\
		"}\n"\
        "</script>\n"\
    "</head>\n"\
    "<body onload='init()'>\n"\
		"<button id='data'  onclick=load('data','led1.cgi') style='width:100px; border-width:2px;' >liga\n"\
	    "</button>\n"\
		"<button id='data2'  onclick=load('data2','led2.cgi') style='width:100px; border-width:2px;' >liga\n"\
	    "</button>\n"\
    "</body>\n"\
"</html>\n"

#define led1_on "<a href='#'>Ligado</a>\n"

#define led1_off "<a href='#'>Desligado</a>\n"

#define led2_on "<a href='#'>Ligado</a>\n"

#define led2_off "<a href='#'>Desligado</a>\n"
