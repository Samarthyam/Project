#include<reg51.h>
#include<stdio.h>
#include<string.h>

#define lcd_data P2

sbit  lcd_rs   =  P2^0;  
sbit  lcd_en   =  P2^1;


sbit cs=P1^0;		//ADC temp
sbit din=P1^1;
sbit dout=P1^2;
sbit clk=P1^3;

//sbit buzzer = P3^7;

idata float voltage_const=3211,voltage_vd_const=8.518,temp=0;
idata float voltage_divider=0;
int voltage_battery=0,voltage_battery1=0;


unsigned char voltage_batterys[10];//voltage_batterys1[10];

int cntl=0,cntl1=0;

unsigned char rcg;

unsigned convert(unsigned int);
unsigned converts(unsigned int);


void delay(unsigned int t)
 {
  unsigned int i,j;
  for(i=0;i<t;i++)
  for(j=0;j<85;j++);
  }

   void lcdcmd(unsigned char value)		   // LCD COMMAND
  {
      lcd_data=value&(0xf0); //send msb 4 bits
      lcd_rs=0;	  //select command register
      lcd_en=1;	  //enable the lcd to execute command
	  delay(3);
	  lcd_en=0;  
      lcd_data=((value<<4)&(0xf0));	 //send lsb 4 bits
      lcd_rs=0;	 //select command register
      lcd_en=1;	 //enable the lcd to execute command
	  delay(3);
	  lcd_en=0;  
      
  }  
void lcd_init(void)
{
 	
 lcdcmd(0x02);
 lcdcmd(0x28);  //intialise the lcd in 4 bit mode*/
 lcdcmd(0x0e);	//cursor blinking
 lcdcmd(0x06);	//move the cursor to right side
 lcdcmd(0x01);	//clear the lcd

}	
void lcddata(unsigned char value)

  {
       
      lcd_data=value&(0xf0); //send msb 4 bits
      lcd_rs=1;	  //select data register
      lcd_en=1;	  //enable the lcd to execute data
	  delay(3);
	  lcd_en=0;  
      lcd_data=((value<<4)&(0xf0));	 //send lsb 4 bits
      lcd_rs=1;	  //select data register
      lcd_en=1;	  //enable the lcd to execute data
	  delay(3);
	  lcd_en=0;  
      
     delay(3); 
  }  



void msgdisplay(unsigned char b[]) // send string to lcd
  {
unsigned char s,count1=0;
for(s=0;b[s]!='\0';s++)
 {
  count1++;	 
  if(s==16)
   lcdcmd(0xc0);
   if(s==32)
   {
   lcdcmd(1);
   count1=0;
   }
  lcddata(b[s]);
 }
}	


void clock()
{
clk=0;
delay(1);
clk=1;
delay(1);
}
void powerup(unsigned char sel)
{
din=0;
clock();clock();clock();clock();clock();

din=1;
clock();//start bit
din=1;
clock();//mode select bit
if(sel==1)din=1;
else{din=0;}
clock();//D2

clock();//D0
clock();//sampling time
}
unsigned int read_mcp3202()
{
unsigned char ii=0;
unsigned int read_i;
clk=0;
delay(1);
clk=1;//for null charecter
read_i=0;
delay(1);
for(ii=0;ii<12;ii++)
{
clk=0;
delay(1);
if(dout==1){read_i++;}
read_i=read_i<<1;
clk=1;
delay(1);
}
return read_i;
}

void serinit()
  {
    TMOD=0x20;
    TH1=0xFD;	  //9600
    SCON=0x50;
    TR1=1;
  }
  
 unsigned char receive()
 {
  unsigned char rx; 
   while(RI == 0);
    rx=SBUF;
	RI=0;
	return rx;
 }

void tx(unsigned char *tx)
 {
    //unsigned char v;
   for(;*tx != '\0';tx++)
     {
	   SBUF=*tx;
	  while(TI == 0);
	   TI=0;
	  // v= receive();
	   //delay(2);
	   
	 }
 }

void tx1(unsigned char tx)
 {
  	   ///unsigned char v;
	   SBUF=tx;
	  while(TI == 0);
	   TI=0;
	     //v= receive();
		 //delay(2);
 }

void okc()
  {
  	do{
	
	rcg=receive();
	}	while(rcg!='K');
  
  }

void wifiinit()
{
	//stringlcd(0x80,"Wifi Initilizing");
	lcdcmd(1);lcdcmd(0x80);msgdisplay("Wifi Initializing");

	tx("AT\r\n");
	//okc();
	delay(800);
	tx("ATE0\r\n");
	okc();
	delay(800);
	tx("AT+CWMODE=3\r\n");
	delay(800);
//	txs("AT+CWSAP=\"org_6327\",\"connectnow\",5,0\r\n");   //1st time enable after disable
//	delay(400);
//     tx("AT+CWJAP=\"projecta\",\"project12345\"\r\n"); 
//	okc();
	//delay(400);
	 tx("AT+CIPMUX=1\r\n");
	 delay(800);
  	 tx("AT+CIPSERVER=1,23\r\n");
	 delay(800);

	lcdcmd(1);msgdisplay("WAITING FOR CONNCT");
	lcdcmd(0xC0);msgdisplay("org_6327");
	 
	 //lcdcmd(0xc0);msgdisplay("org_6547");
	while(receive()!='C');
	lcdcmd(1);lcdcmd(0x80);msgdisplay("Connected");
}


void sendwifi(unsigned char *chr,unsigned int length)
{
unsigned char temp[20];
	tx("AT+CIPSEND=0,");
	sprintf(temp,"%u",length);
	tx(temp);
	tx("\r\n");
	delay(600);
	tx("vin:");
	tx(chr);
	delay(400);
}





 void main()
{
  unsigned int cntl=0,len1=0;

  P2=0xff;

  serinit();

   clk=0;
   din=0;
   dout=0;
   cs=0;

   
//   relay=0;buzzer=1;
	
lcd_init();
lcdcmd(1);
msgdisplay("Foot Step Power Generation");
delay(800);

   wifiinit();

tx("AT+CIPSEND=0,28\r\n");delay(600);
tx("Foot Step Power Generation\r\n");

	delay(900);
   lcdcmd(1);


msgdisplay("Vin:"); //0x85
lcdcmd(0xc0);
msgdisplay("Vout:");//0xc5

while(1)
{
 for(cntl1=0;cntl1<15;cntl1++)
    {
     cs=1;
     delay(10);
     cs=0;
     delay(10);
     powerup(0);
     //sertxs("TEMP:");
     temp = read_mcp3202();
	 voltage_divider = (temp/voltage_const);	
	 voltage_battery = (voltage_divider*voltage_vd_const);
	 //memset(voltage_batterys,'\0',10);

     for(cntl=0;cntl<10;cntl++)voltage_batterys[cntl] = '\0';
	//volt1 =  voltage_battery;
	 //sprintf(voltage_batterys,"%f",voltage_battery);
	 lcdcmd(0x85);
     convert(voltage_battery);
	 //msgdisplay(voltage_batterys);
	 delay(10);

	 
     cs=1;
     delay(10);
     cs=0;
     delay(10);
     powerup(1);
     //sertxs("TEMP:");
     temp = read_mcp3202();
	 voltage_divider = (temp/voltage_const);	
	 voltage_battery1 = (voltage_divider*voltage_vd_const);
	 //memset(voltage_batterys,'\0',10);

     //for(cntl=0;cntl<10;cntl++)voltage_batterys1[cntl] = '\0';
	 //volt2 =  voltage_battery;
	 //sprintf(voltage_batterys1,"%f",voltage_battery);
	 lcdcmd(0xc6);
     convert(voltage_battery1);
	 //msgdisplay(voltage_batterys1);
	 delay(10);
	}

	
	//len1 = strlen(voltage_batterys);
	//len1 = (len1+4);
	//sendwifi(voltage_batterys,len1);
   tx("AT+CIPSEND=0,22\r\n");delay(600);
   tx("Vin:");converts(voltage_battery);tx(" Vout:");converts(voltage_battery1);tx("\r\n");delay(700);
  }
}






unsigned convert(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;							 
      c=c|0x30;
      e=e|0x30; 
	  g=g|0x30;							 
      h=h|0x30;
    

     
	 lcddata(a);
	 lcddata(c);
	// lcddata('.');
	 lcddata(e); lcddata(g);lcddata(h);//lcddata('V');//lcddata(' ');lcddata(' ');
	   
	   return 1;
}

unsigned converts(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;							 
      c=c|0x30;
      e=e|0x30; 
	  g=g|0x30;							 
      h=h|0x30;
    

     
	 tx1(a);
	 tx1(c);
	 tx1(e); tx1(g);tx1(h);//tx1('V');//lcddata(' ');lcddata(' ');
	   
	   return 1;
}
