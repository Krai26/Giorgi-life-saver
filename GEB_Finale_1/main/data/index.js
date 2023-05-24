 
 "use strict";

 
 var alarmFlag,str;
 var snd = new Audio("alarm.mp3");
 var sogliaTemperatura,sogliaUmidita,sogliaCo2,sogliaCo;
 
 
 function alarm(alarmFlag,str){
  //inserire qui il codice di controllo che tramite i codPer visualizza gli allarmi negli appositi frame id "Alarm1, Alarm2.....etc" 
  
  
     
   if(alarmFlag){
	  document.getElementById("Alarm1").innerHTML ="<center><div style='background-color:red; border: 2px solid black; height:50px; width:500px; font-face: Times New Roman; text-align: center; font-size: 36px; color:yellow;' >Allarme "+str+"!<br></div><br>";
		        
   snd.play(); 
   }else {
	  document.getElementById("Alarm1").innerHTML ="<center><div style='background-color:green; border: 2px solid black; height:50px; width:500px; font-face: Times New Roman; text-align: center; font-size: 48px; color:yellow;' >Ok<br></div><br>";
  }
  
 }
 

 

 
function getSogliaTemperatura(){
	setTimeout(getSogliaTemperatura, 20000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	if (xhttp.readyState == 4 && xhttp.status == 200) {

	   sogliaTemperatura = xhttp.responseText;
	   console.log(sogliaTemperatura);
	   
	   		
	}
	  
   
   } 
   xhttp.open("GET","http://192.168.10.30:8080/dev/getSogliaTemperatura/", true);
   xhttp.send();
	 
   
}

function getTemp(){
	setTimeout(getTemp, 6000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	 if (xhttp.readyState == 4 && xhttp.status == 200) {
	   let strb = xhttp.responseText;
	   document.getElementById("temp_id1").innerHTML =strb;
	      
				 
		 console.log(strb);
		 		 
		if(parseInt(strb)>parseInt(sogliaTemperatura)){
		 
		 
		 alarm(true,"Temperatura");
		 
		 
		}
		 
		 
		 
		 
				  
		
	  }
	  /*else {// keep alive server
		  counterMonitorAlarm++//contatore del tempo in cui rimane sconnesso il server per generare un eventuale alert
		  //console.log("server stop counter "+counterMonitorAlarm);//debug
		  if(counterMonitorAlarm>=60){document.getElementById("kalive").innerHTML =("<div style='background-color:red; border: 2px solid black; height:25px; width:25px; font-face: Times New Roman; text-align: center; font-size: 20px; color: blue;'></div>");
		  
		  alarm(false);
		  
		  }
	  }*/
	}
	  
   
   
   xhttp.open("GET","http://192.168.10.30:8080/dev/getTemp/", true);
   xhttp.send();
	 
 }



 function getSogliaUmidita(){
	setTimeout(getSogliaUmidita, 20000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	if (xhttp.readyState == 4 && xhttp.status == 200) {

	   sogliaUmidita = xhttp.responseText;
	   console.log(sogliaUmidita);
	   
	   		
	}
	  
   
   } 
   xhttp.open("GET","http://192.168.10.30:8080/dev/getSogliaUmidita/", true);
   xhttp.send();
	 
   
}
 function getUmidita(){
	setTimeout(getUmidita, 6000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	 if (xhttp.readyState == 4 && xhttp.status == 200) {
	   let strb = xhttp.responseText;
	   document.getElementById("hum_id1").innerHTML =strb;
	      
				 
		 console.log(strb);
		 
		if(parseInt(strb)>parseInt(sogliaUmidita)){
		 
		 
		 alarm(true,"Umidita'");
		 
		 
		}
		 
		 
		 
		 
				  
		
	  }
	  /*else {// keep alive server
		  counterMonitorAlarm++//contatore del tempo in cui rimane sconnesso il server per generare un eventuale alert
		  //console.log("server stop counter "+counterMonitorAlarm);//debug
		  if(counterMonitorAlarm>=60){document.getElementById("kalive").innerHTML =("<div style='background-color:red; border: 2px solid black; height:25px; width:25px; font-face: Times New Roman; text-align: center; font-size: 20px; color: blue;'></div>");
		  
		  alarm(false);
		  
		  }
	  }*/
	}
	  
   
   
   xhttp.open("GET","http://192.168.10.30:8080/dev/getUmidita/", true);
   xhttp.send();
	 
 }


 function getSogliaCo2(){
	setTimeout(getSogliaCo2, 20000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	if (xhttp.readyState == 4 && xhttp.status == 200) {

	   sogliaCo2 = xhttp.responseText;
	   console.log(sogliaCo2);
	   
	   		
	}
	  
   
   } 
   xhttp.open("GET","http://192.168.10.30:8080/dev/getSogliaCo2/", true);
   xhttp.send();
	 
   
}


 function getCo2(){
	setTimeout(getCo2, 6000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	 if (xhttp.readyState == 4 && xhttp.status == 200) {
	   let strb = xhttp.responseText;
	   document.getElementById("co2_id1").innerHTML =strb;
	   
		 
	   
				 
		 console.log(strb);
		 //console.log(strb);
		 
		if(parseInt(strb)>sogliaCo2){
		 
		 
		 alarm(true,"CO2");
		 
		 
		}
		 
		 
		 
				  
		
	  }
	  /*else {// keep alive server
		  counterMonitorAlarm++//contatore del tempo in cui rimane sconnesso il server per generare un eventuale alert
		  //console.log("server stop counter "+counterMonitorAlarm);//debug
		  if(counterMonitorAlarm>=60){document.getElementById("kalive").innerHTML =("<div style='background-color:red; border: 2px solid black; height:25px; width:25px; font-face: Times New Roman; text-align: center; font-size: 20px; color: blue;'></div>");
		  
		  alarm(false);
		  
		  }
	  }*/
	}
	  
   
   
   xhttp.open("GET","http://192.168.10.30:8080/dev/getCo2/", true);
   xhttp.send();
	 
 }


 function getSogliaCo(){
	setTimeout(getSogliaCo, 20000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	if (xhttp.readyState == 4 && xhttp.status == 200) {

	   sogliaCo = xhttp.responseText;
	   console.log(sogliaCo);
	   
	   		
	}
	  
   
   } 
   xhttp.open("GET","http://192.168.10.30:8080/dev/getSogliaCo/", true);
   xhttp.send();
	 
   
}


 function getCo(){
	setTimeout(getCo, 6000);
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	 
	 if (xhttp.readyState == 4 && xhttp.status == 200) {
	   let strb = xhttp.responseText;
	   document.getElementById("co_id1").innerHTML =strb;
	   
		 
	   
				 
		 console.log(strb);
		 //console.log(strb);
		 
		if(parseInt(strb)>sogliaCo){
		 
		 
		 alarm(true,"CO");
		 
		 
		}
		
		 
		 
				  
		
	  }
	  /*else {// keep alive server
		  counterMonitorAlarm++//contatore del tempo in cui rimane sconnesso il server per generare un eventuale alert
		  //console.log("server stop counter "+counterMonitorAlarm);//debug
		  if(counterMonitorAlarm>=60){document.getElementById("kalive").innerHTML =("<div style='background-color:red; border: 2px solid black; height:25px; width:25px; font-face: Times New Roman; text-align: center; font-size: 20px; color: blue;'></div>");
		  
		  alarm(false);
		  
		  }
	  }*/
	}
	  
   
   
   xhttp.open("GET","http://192.168.10.30:8080/dev/getCo/", true);
   xhttp.send();
	 
 }

getSogliaTemperatura();
getTemp();
getSogliaUmidita();
getUmidita();
getSogliaCo2();
getCo2();
getSogliaCo();
getCo();


 
 
 
 
 
 
 
 
 
