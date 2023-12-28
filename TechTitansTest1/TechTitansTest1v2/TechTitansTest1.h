#ifndef CUSTOM_PDO_NAME_H
#define CUSTOM_PDO_NAME_H

//-------------------------------------------------------------------//
//                                                                   //
//     This file has been created by the Easy Configurator tool      //
//                                                                   //
//     Easy Configurator project TechTitansTest1.prj
//                                                                   //
//-------------------------------------------------------------------//


#define CUST_BYTE_NUM_OUT	32
#define CUST_BYTE_NUM_IN	32
#define TOT_BYTE_NUM_ROUND_OUT	32
#define TOT_BYTE_NUM_ROUND_IN	32


typedef union												//---- output buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_OUT];
	struct
	{
		uint8_t     Byte0;
		uint8_t     Byte1;
		uint8_t     Byte2;
		uint8_t     Byte3;
		uint8_t     Byte4;
		uint8_t     Byte5;
		uint8_t     Byte6;
		uint8_t     Byte7;
		uint8_t     Byte8;
		uint8_t     Byte9;
		uint8_t     Byte10;
		uint8_t     Byte11;
		uint8_t     Byte12;
		uint8_t     Byte13;
		uint8_t     Byte14;
		uint8_t     Byte15;
		uint8_t     Byte16;
		uint8_t     Byte17;
		uint8_t     Byte18;
		uint8_t     Byte19;
		uint8_t     Byte20;
		uint8_t     Byte21;
		uint8_t     Byte22;
		uint8_t     Byte23;
		uint8_t     Byte24;
		uint8_t     Byte25;
		uint8_t     Byte26;
		uint8_t     Byte27;
		uint8_t     Byte28;
		uint8_t     Byte29;
		uint8_t     Byte30;
		uint8_t     Byte31;
	}Cust;
} PROCBUFFER_OUT;


typedef union												//---- input buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_IN];
	struct
	{
		uint8_t     Byte0;
		uint8_t     Byte1;
		uint8_t     Byte2;
		uint8_t     Byte3;
		uint8_t     Byte4;
		uint8_t     Byte5;
		uint8_t     Byte6;
		uint8_t     Byte7;
		uint8_t     Byte8;
		uint8_t     Byte9;
		uint8_t     Byte10;
		uint8_t     Byte11;
		uint8_t     Byte12;
		uint8_t     Byte13;
		uint8_t     Byte14;
		uint8_t     Byte15;
		uint8_t     Byte16;
		uint8_t     Byte17;
		uint8_t     Byte18;
		uint8_t     Byte19;
		uint8_t     Byte20;
		uint8_t     Byte21;
		uint8_t     Byte22;
		uint8_t     Byte23;
		uint8_t     Byte24;
		uint8_t     Byte25;
		uint8_t     Byte26;
		uint8_t     Byte27;
		uint8_t     Byte28;
		uint8_t     Byte29;
		uint8_t     Byte30;
		uint8_t     Byte31;
	}Cust;
} PROCBUFFER_IN;

#endif