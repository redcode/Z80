/* Z80 v0.2
       ______  ______ ______
      /\___  \/\  __ \\  __ \
 ____ \/__/  /\_\  __ \\ \/\ \ ________________________________________________
|        /\_____\\_____\\_____\                                                |
|  Zilog \/_____//_____//_____/ CPU Emulator                                   |
|  Copyright (C) 1999-2025 Manuel Sainz de Baranda y Goñi.                     |
|                                                                              |
|  This emulator is free software: you can redistribute it and/or modify it    |
|  under the terms of the GNU Lesser General Public License as published by    |
|  the Free Software Foundation, either version 3 of the License, or (at your  |
|  option) any later version.                                                  |
|                                                                              |
|  This emulator is distributed in the hope that it will be useful, but        |
|  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  |
|  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public      |
|  License for more details.                                                   |
|                                                                              |
|  You should have received a copy of the GNU Lesser General Public License    |
|  along with this emulator. If not, see <http://www.gnu.org/licenses/>.       |
|                                                                              |
|  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  |
|                                                                              |
|  A NOTE FROM THE ORIGINAL AUTHOR                                             |
|                                                                              |
|  Those familiar with the official documentation of the Zilog Z80 CPU will    |
|  find this source code quite intuitive. The purpose has not been to write    |
|  the fastest possible emulator, although the speed aspect is not neglected,  |
|                                          but a portable, hackable and well-  |
|             .----._.----.                structured piece of software;       |
|   A11 <-01--|1         o|--40-> A10      something small, solid and elegant  |
|   A12 <-02--|           |--39-> A09      that can stand the test of time     |
|   A13 <-03--|           |--38-> A08      with no need for major changes.     |
|   A14 <-04--|           |--37-> A07                                          |
|   A15 <-05--|           |--36-> A06      Some of the main design decisions   |
|   CLK --06->|           |--35-> A05      have been the following:            |
|    D4 <-07->|           |--34-> A04                                          |
|    D3 <-08->|.---------.|--33-> A03      1. Opcode partial decoding keeps    |
|    D5 <-09->|| ZILOG   ||--32-> A02      the code small and maintainable.    |
|    D6 <-10->|| Z80     ||--31-> A01                                          |
|   +5V --11--|| CPU     ||--30-> A00      2. Function pointer tables for      |
|    D2 <-12->||         ||--29-- GND      opcode selection allow easy reuse   |
|    D7 <-13->|'---------'|--28-> RFSH     of almost all instruction code      |
|    D0 <-14->|           |--27-> M1       in the interrupt mode 0.            |
|    D1 <-15->|           |<-26-- RESET                                        |
|   INT --16->|           |<-25-- BUSREQ   3. Avoiding conditional statements  |
|   NMI --17->|           |<-24-- WAIT     as much as possible reduces the     |
|  HALT <-18--|           |--23-> BUSACK   branch penalty in modern pipelined  |
|  MREQ <-19--|           |--22-> WR       processors.                         |
|  IORQ <-20--|           |--21-> RD                                           |
|             '-----------'                                                    |
|    Zilog Z80 CPU, May 1976 version                                           |
|       40-pin ceramic DIP pinout                                      Manuel  |
|                                                                              |
'=============================================================================*/

#ifndef Z80_EXTERNAL_HEADER
#	include <Z/constants/pointer.h>
#	include <Z/macros/bitwise.h>
#	include <Z/macros/structure.h>
#endif

#ifdef Z80_STATIC
#	define Z80_API
#else
#	define Z80_API Z_API_EXPORT
#endif

#ifdef Z80_WITH_LOCAL_HEADER
#	include "Z80.h"
#else
#	include <Z80.h>
#endif


/* MARK: - Precomputed Values of AF for `daa` */

#ifdef Z80_WITH_PRECOMPUTED_DAA
#	define H(value) Z_UINT16(0x##value)

	static zuint16 const daa_af_table[2048] = {
	/* HNC */
	/* 000	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(0044), H(0100), H(0200), H(0304), H(0400), H(0504), H(0604), H(0700), H(0808), H(090C), H(1010), H(1114), H(1214), H(1310), H(1414), H(1510),
	/* 1 */ H(1000), H(1104), H(1204), H(1300), H(1404), H(1500), H(1600), H(1704), H(180C), H(1908), H(2030), H(2134), H(2234), H(2330), H(2434), H(2530),
	/* 2 */ H(2020), H(2124), H(2224), H(2320), H(2424), H(2520), H(2620), H(2724), H(282C), H(2928), H(3034), H(3130), H(3230), H(3334), H(3430), H(3534),
	/* 3 */ H(3024), H(3120), H(3220), H(3324), H(3420), H(3524), H(3624), H(3720), H(3828), H(392C), H(4010), H(4114), H(4214), H(4310), H(4414), H(4510),
	/* 4 */ H(4000), H(4104), H(4204), H(4300), H(4404), H(4500), H(4600), H(4704), H(480C), H(4908), H(5014), H(5110), H(5210), H(5314), H(5410), H(5514),
	/* 5 */ H(5004), H(5100), H(5200), H(5304), H(5400), H(5504), H(5604), H(5700), H(5808), H(590C), H(6034), H(6130), H(6230), H(6334), H(6430), H(6534),
	/* 6 */ H(6024), H(6120), H(6220), H(6324), H(6420), H(6524), H(6624), H(6720), H(6828), H(692C), H(7030), H(7134), H(7234), H(7330), H(7434), H(7530),
	/* 7 */ H(7020), H(7124), H(7224), H(7320), H(7424), H(7520), H(7620), H(7724), H(782C), H(7928), H(8090), H(8194), H(8294), H(8390), H(8494), H(8590),
	/* 8 */ H(8080), H(8184), H(8284), H(8380), H(8484), H(8580), H(8680), H(8784), H(888C), H(8988), H(9094), H(9190), H(9290), H(9394), H(9490), H(9594),
	/* 9 */ H(9084), H(9180), H(9280), H(9384), H(9480), H(9584), H(9684), H(9780), H(9888), H(998C), H(0055), H(0111), H(0211), H(0315), H(0411), H(0515),
	/* A */ H(0045), H(0101), H(0201), H(0305), H(0401), H(0505), H(0605), H(0701), H(0809), H(090D), H(1011), H(1115), H(1215), H(1311), H(1415), H(1511),
	/* B */ H(1001), H(1105), H(1205), H(1301), H(1405), H(1501), H(1601), H(1705), H(180D), H(1909), H(2031), H(2135), H(2235), H(2331), H(2435), H(2531),
	/* C */ H(2021), H(2125), H(2225), H(2321), H(2425), H(2521), H(2621), H(2725), H(282D), H(2929), H(3035), H(3131), H(3231), H(3335), H(3431), H(3535),
	/* D */ H(3025), H(3121), H(3221), H(3325), H(3421), H(3525), H(3625), H(3721), H(3829), H(392D), H(4011), H(4115), H(4215), H(4311), H(4415), H(4511),
	/* E */ H(4001), H(4105), H(4205), H(4301), H(4405), H(4501), H(4601), H(4705), H(480D), H(4909), H(5015), H(5111), H(5211), H(5315), H(5411), H(5515),
	/* F */ H(5005), H(5101), H(5201), H(5305), H(5401), H(5505), H(5605), H(5701), H(5809), H(590D), H(6035), H(6131), H(6231), H(6335), H(6431), H(6535),
	/* HNC */
	/* 001	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(6025), H(6121), H(6221), H(6325), H(6421), H(6525), H(6625), H(6721), H(6829), H(692D), H(7031), H(7135), H(7235), H(7331), H(7435), H(7531),
	/* 1 */ H(7021), H(7125), H(7225), H(7321), H(7425), H(7521), H(7621), H(7725), H(782D), H(7929), H(8091), H(8195), H(8295), H(8391), H(8495), H(8591),
	/* 2 */ H(8081), H(8185), H(8285), H(8381), H(8485), H(8581), H(8681), H(8785), H(888D), H(8989), H(9095), H(9191), H(9291), H(9395), H(9491), H(9595),
	/* 3 */ H(9085), H(9181), H(9281), H(9385), H(9481), H(9585), H(9685), H(9781), H(9889), H(998D), H(A0B5), H(A1B1), H(A2B1), H(A3B5), H(A4B1), H(A5B5),
	/* 4 */ H(A0A5), H(A1A1), H(A2A1), H(A3A5), H(A4A1), H(A5A5), H(A6A5), H(A7A1), H(A8A9), H(A9AD), H(B0B1), H(B1B5), H(B2B5), H(B3B1), H(B4B5), H(B5B1),
	/* 5 */ H(B0A1), H(B1A5), H(B2A5), H(B3A1), H(B4A5), H(B5A1), H(B6A1), H(B7A5), H(B8AD), H(B9A9), H(C095), H(C191), H(C291), H(C395), H(C491), H(C595),
	/* 6 */ H(C085), H(C181), H(C281), H(C385), H(C481), H(C585), H(C685), H(C781), H(C889), H(C98D), H(D091), H(D195), H(D295), H(D391), H(D495), H(D591),
	/* 7 */ H(D081), H(D185), H(D285), H(D381), H(D485), H(D581), H(D681), H(D785), H(D88D), H(D989), H(E0B1), H(E1B5), H(E2B5), H(E3B1), H(E4B5), H(E5B1),
	/* 8 */ H(E0A1), H(E1A5), H(E2A5), H(E3A1), H(E4A5), H(E5A1), H(E6A1), H(E7A5), H(E8AD), H(E9A9), H(F0B5), H(F1B1), H(F2B1), H(F3B5), H(F4B1), H(F5B5),
	/* 9 */ H(F0A5), H(F1A1), H(F2A1), H(F3A5), H(F4A1), H(F5A5), H(F6A5), H(F7A1), H(F8A9), H(F9AD), H(0055), H(0111), H(0211), H(0315), H(0411), H(0515),
	/* A */ H(0045), H(0101), H(0201), H(0305), H(0401), H(0505), H(0605), H(0701), H(0809), H(090D), H(1011), H(1115), H(1215), H(1311), H(1415), H(1511),
	/* B */ H(1001), H(1105), H(1205), H(1301), H(1405), H(1501), H(1601), H(1705), H(180D), H(1909), H(2031), H(2135), H(2235), H(2331), H(2435), H(2531),
	/* C */ H(2021), H(2125), H(2225), H(2321), H(2425), H(2521), H(2621), H(2725), H(282D), H(2929), H(3035), H(3131), H(3231), H(3335), H(3431), H(3535),
	/* D */ H(3025), H(3121), H(3221), H(3325), H(3421), H(3525), H(3625), H(3721), H(3829), H(392D), H(4011), H(4115), H(4215), H(4311), H(4415), H(4511),
	/* E */ H(4001), H(4105), H(4205), H(4301), H(4405), H(4501), H(4601), H(4705), H(480D), H(4909), H(5015), H(5111), H(5211), H(5315), H(5411), H(5515),
	/* F */ H(5005), H(5101), H(5201), H(5305), H(5401), H(5505), H(5605), H(5701), H(5809), H(590D), H(6035), H(6131), H(6231), H(6335), H(6431), H(6535),
	/* HNC */
	/* 010	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(0046), H(0102), H(0202), H(0306), H(0402), H(0506), H(0606), H(0702), H(080A), H(090E), H(0402), H(0506), H(0606), H(0702), H(080A), H(090E),
	/* 1 */ H(1002), H(1106), H(1206), H(1302), H(1406), H(1502), H(1602), H(1706), H(180E), H(190A), H(1406), H(1502), H(1602), H(1706), H(180E), H(190A),
	/* 2 */ H(2022), H(2126), H(2226), H(2322), H(2426), H(2522), H(2622), H(2726), H(282E), H(292A), H(2426), H(2522), H(2622), H(2726), H(282E), H(292A),
	/* 3 */ H(3026), H(3122), H(3222), H(3326), H(3422), H(3526), H(3626), H(3722), H(382A), H(392E), H(3422), H(3526), H(3626), H(3722), H(382A), H(392E),
	/* 4 */ H(4002), H(4106), H(4206), H(4302), H(4406), H(4502), H(4602), H(4706), H(480E), H(490A), H(4406), H(4502), H(4602), H(4706), H(480E), H(490A),
	/* 5 */ H(5006), H(5102), H(5202), H(5306), H(5402), H(5506), H(5606), H(5702), H(580A), H(590E), H(5402), H(5506), H(5606), H(5702), H(580A), H(590E),
	/* 6 */ H(6026), H(6122), H(6222), H(6326), H(6422), H(6526), H(6626), H(6722), H(682A), H(692E), H(6422), H(6526), H(6626), H(6722), H(682A), H(692E),
	/* 7 */ H(7022), H(7126), H(7226), H(7322), H(7426), H(7522), H(7622), H(7726), H(782E), H(792A), H(7426), H(7522), H(7622), H(7726), H(782E), H(792A),
	/* 8 */ H(8082), H(8186), H(8286), H(8382), H(8486), H(8582), H(8682), H(8786), H(888E), H(898A), H(8486), H(8582), H(8682), H(8786), H(888E), H(898A),
	/* 9 */ H(9086), H(9182), H(9282), H(9386), H(9482), H(9586), H(9686), H(9782), H(988A), H(998E), H(3423), H(3527), H(3627), H(3723), H(382B), H(392F),
	/* A */ H(4003), H(4107), H(4207), H(4303), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B),
	/* B */ H(5007), H(5103), H(5203), H(5307), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F),
	/* C */ H(6027), H(6123), H(6223), H(6327), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F),
	/* D */ H(7023), H(7127), H(7227), H(7323), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B),
	/* E */ H(8083), H(8187), H(8287), H(8383), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B),
	/* F */ H(9087), H(9183), H(9283), H(9387), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F),
	/* HNC */
	/* 011	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(A0A7), H(A1A3), H(A2A3), H(A3A7), H(A4A3), H(A5A7), H(A6A7), H(A7A3), H(A8AB), H(A9AF), H(A4A3), H(A5A7), H(A6A7), H(A7A3), H(A8AB), H(A9AF),
	/* 1 */ H(B0A3), H(B1A7), H(B2A7), H(B3A3), H(B4A7), H(B5A3), H(B6A3), H(B7A7), H(B8AF), H(B9AB), H(B4A7), H(B5A3), H(B6A3), H(B7A7), H(B8AF), H(B9AB),
	/* 2 */ H(C087), H(C183), H(C283), H(C387), H(C483), H(C587), H(C687), H(C783), H(C88B), H(C98F), H(C483), H(C587), H(C687), H(C783), H(C88B), H(C98F),
	/* 3 */ H(D083), H(D187), H(D287), H(D383), H(D487), H(D583), H(D683), H(D787), H(D88F), H(D98B), H(D487), H(D583), H(D683), H(D787), H(D88F), H(D98B),
	/* 4 */ H(E0A3), H(E1A7), H(E2A7), H(E3A3), H(E4A7), H(E5A3), H(E6A3), H(E7A7), H(E8AF), H(E9AB), H(E4A7), H(E5A3), H(E6A3), H(E7A7), H(E8AF), H(E9AB),
	/* 5 */ H(F0A7), H(F1A3), H(F2A3), H(F3A7), H(F4A3), H(F5A7), H(F6A7), H(F7A3), H(F8AB), H(F9AF), H(F4A3), H(F5A7), H(F6A7), H(F7A3), H(F8AB), H(F9AF),
	/* 6 */ H(0047), H(0103), H(0203), H(0307), H(0403), H(0507), H(0607), H(0703), H(080B), H(090F), H(0403), H(0507), H(0607), H(0703), H(080B), H(090F),
	/* 7 */ H(1003), H(1107), H(1207), H(1303), H(1407), H(1503), H(1603), H(1707), H(180F), H(190B), H(1407), H(1503), H(1603), H(1707), H(180F), H(190B),
	/* 8 */ H(2023), H(2127), H(2227), H(2323), H(2427), H(2523), H(2623), H(2727), H(282F), H(292B), H(2427), H(2523), H(2623), H(2727), H(282F), H(292B),
	/* 9 */ H(3027), H(3123), H(3223), H(3327), H(3423), H(3527), H(3627), H(3723), H(382B), H(392F), H(3423), H(3527), H(3627), H(3723), H(382B), H(392F),
	/* A */ H(4003), H(4107), H(4207), H(4303), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B),
	/* B */ H(5007), H(5103), H(5203), H(5307), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F),
	/* C */ H(6027), H(6123), H(6223), H(6327), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F),
	/* D */ H(7023), H(7127), H(7227), H(7323), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B),
	/* E */ H(8083), H(8187), H(8287), H(8383), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B),
	/* F */ H(9087), H(9183), H(9283), H(9387), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F),
	/* HNC */
	/* 100	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(0604), H(0700), H(0808), H(090C), H(0A0C), H(0B08), H(0C0C), H(0D08), H(0E08), H(0F0C), H(1010), H(1114), H(1214), H(1310), H(1414), H(1510),
	/* 1 */ H(1600), H(1704), H(180C), H(1908), H(1A08), H(1B0C), H(1C08), H(1D0C), H(1E0C), H(1F08), H(2030), H(2134), H(2234), H(2330), H(2434), H(2530),
	/* 2 */ H(2620), H(2724), H(282C), H(2928), H(2A28), H(2B2C), H(2C28), H(2D2C), H(2E2C), H(2F28), H(3034), H(3130), H(3230), H(3334), H(3430), H(3534),
	/* 3 */ H(3624), H(3720), H(3828), H(392C), H(3A2C), H(3B28), H(3C2C), H(3D28), H(3E28), H(3F2C), H(4010), H(4114), H(4214), H(4310), H(4414), H(4510),
	/* 4 */ H(4600), H(4704), H(480C), H(4908), H(4A08), H(4B0C), H(4C08), H(4D0C), H(4E0C), H(4F08), H(5014), H(5110), H(5210), H(5314), H(5410), H(5514),
	/* 5 */ H(5604), H(5700), H(5808), H(590C), H(5A0C), H(5B08), H(5C0C), H(5D08), H(5E08), H(5F0C), H(6034), H(6130), H(6230), H(6334), H(6430), H(6534),
	/* 6 */ H(6624), H(6720), H(6828), H(692C), H(6A2C), H(6B28), H(6C2C), H(6D28), H(6E28), H(6F2C), H(7030), H(7134), H(7234), H(7330), H(7434), H(7530),
	/* 7 */ H(7620), H(7724), H(782C), H(7928), H(7A28), H(7B2C), H(7C28), H(7D2C), H(7E2C), H(7F28), H(8090), H(8194), H(8294), H(8390), H(8494), H(8590),
	/* 8 */ H(8680), H(8784), H(888C), H(8988), H(8A88), H(8B8C), H(8C88), H(8D8C), H(8E8C), H(8F88), H(9094), H(9190), H(9290), H(9394), H(9490), H(9594),
	/* 9 */ H(9684), H(9780), H(9888), H(998C), H(9A8C), H(9B88), H(9C8C), H(9D88), H(9E88), H(9F8C), H(0055), H(0111), H(0211), H(0315), H(0411), H(0515),
	/* A */ H(0605), H(0701), H(0809), H(090D), H(0A0D), H(0B09), H(0C0D), H(0D09), H(0E09), H(0F0D), H(1011), H(1115), H(1215), H(1311), H(1415), H(1511),
	/* B */ H(1601), H(1705), H(180D), H(1909), H(1A09), H(1B0D), H(1C09), H(1D0D), H(1E0D), H(1F09), H(2031), H(2135), H(2235), H(2331), H(2435), H(2531),
	/* C */ H(2621), H(2725), H(282D), H(2929), H(2A29), H(2B2D), H(2C29), H(2D2D), H(2E2D), H(2F29), H(3035), H(3131), H(3231), H(3335), H(3431), H(3535),
	/* D */ H(3625), H(3721), H(3829), H(392D), H(3A2D), H(3B29), H(3C2D), H(3D29), H(3E29), H(3F2D), H(4011), H(4115), H(4215), H(4311), H(4415), H(4511),
	/* E */ H(4601), H(4705), H(480D), H(4909), H(4A09), H(4B0D), H(4C09), H(4D0D), H(4E0D), H(4F09), H(5015), H(5111), H(5211), H(5315), H(5411), H(5515),
	/* F */ H(5605), H(5701), H(5809), H(590D), H(5A0D), H(5B09), H(5C0D), H(5D09), H(5E09), H(5F0D), H(6035), H(6131), H(6231), H(6335), H(6431), H(6535),
	/* HNC */
	/* 101	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(6625), H(6721), H(6829), H(692D), H(6A2D), H(6B29), H(6C2D), H(6D29), H(6E29), H(6F2D), H(7031), H(7135), H(7235), H(7331), H(7435), H(7531),
	/* 1 */ H(7621), H(7725), H(782D), H(7929), H(7A29), H(7B2D), H(7C29), H(7D2D), H(7E2D), H(7F29), H(8091), H(8195), H(8295), H(8391), H(8495), H(8591),
	/* 2 */ H(8681), H(8785), H(888D), H(8989), H(8A89), H(8B8D), H(8C89), H(8D8D), H(8E8D), H(8F89), H(9095), H(9191), H(9291), H(9395), H(9491), H(9595),
	/* 3 */ H(9685), H(9781), H(9889), H(998D), H(9A8D), H(9B89), H(9C8D), H(9D89), H(9E89), H(9F8D), H(A0B5), H(A1B1), H(A2B1), H(A3B5), H(A4B1), H(A5B5),
	/* 4 */ H(A6A5), H(A7A1), H(A8A9), H(A9AD), H(AAAD), H(ABA9), H(ACAD), H(ADA9), H(AEA9), H(AFAD), H(B0B1), H(B1B5), H(B2B5), H(B3B1), H(B4B5), H(B5B1),
	/* 5 */ H(B6A1), H(B7A5), H(B8AD), H(B9A9), H(BAA9), H(BBAD), H(BCA9), H(BDAD), H(BEAD), H(BFA9), H(C095), H(C191), H(C291), H(C395), H(C491), H(C595),
	/* 6 */ H(C685), H(C781), H(C889), H(C98D), H(CA8D), H(CB89), H(CC8D), H(CD89), H(CE89), H(CF8D), H(D091), H(D195), H(D295), H(D391), H(D495), H(D591),
	/* 7 */ H(D681), H(D785), H(D88D), H(D989), H(DA89), H(DB8D), H(DC89), H(DD8D), H(DE8D), H(DF89), H(E0B1), H(E1B5), H(E2B5), H(E3B1), H(E4B5), H(E5B1),
	/* 8 */ H(E6A1), H(E7A5), H(E8AD), H(E9A9), H(EAA9), H(EBAD), H(ECA9), H(EDAD), H(EEAD), H(EFA9), H(F0B5), H(F1B1), H(F2B1), H(F3B5), H(F4B1), H(F5B5),
	/* 9 */ H(F6A5), H(F7A1), H(F8A9), H(F9AD), H(FAAD), H(FBA9), H(FCAD), H(FDA9), H(FEA9), H(FFAD), H(0055), H(0111), H(0211), H(0315), H(0411), H(0515),
	/* A */ H(0605), H(0701), H(0809), H(090D), H(0A0D), H(0B09), H(0C0D), H(0D09), H(0E09), H(0F0D), H(1011), H(1115), H(1215), H(1311), H(1415), H(1511),
	/* B */ H(1601), H(1705), H(180D), H(1909), H(1A09), H(1B0D), H(1C09), H(1D0D), H(1E0D), H(1F09), H(2031), H(2135), H(2235), H(2331), H(2435), H(2531),
	/* C */ H(2621), H(2725), H(282D), H(2929), H(2A29), H(2B2D), H(2C29), H(2D2D), H(2E2D), H(2F29), H(3035), H(3131), H(3231), H(3335), H(3431), H(3535),
	/* D */ H(3625), H(3721), H(3829), H(392D), H(3A2D), H(3B29), H(3C2D), H(3D29), H(3E29), H(3F2D), H(4011), H(4115), H(4215), H(4311), H(4415), H(4511),
	/* E */ H(4601), H(4705), H(480D), H(4909), H(4A09), H(4B0D), H(4C09), H(4D0D), H(4E0D), H(4F09), H(5015), H(5111), H(5211), H(5315), H(5411), H(5515),
	/* F */ H(5605), H(5701), H(5809), H(590D), H(5A0D), H(5B09), H(5C0D), H(5D09), H(5E09), H(5F0D), H(6035), H(6131), H(6231), H(6335), H(6431), H(6535),
	/* HNC */
	/* 110	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(FABE), H(FBBA), H(FCBE), H(FDBA), H(FEBA), H(FFBE), H(0046), H(0102), H(0202), H(0306), H(0402), H(0506), H(0606), H(0702), H(080A), H(090E),
	/* 1 */ H(0A1E), H(0B1A), H(0C1E), H(0D1A), H(0E1A), H(0F1E), H(1002), H(1106), H(1206), H(1302), H(1406), H(1502), H(1602), H(1706), H(180E), H(190A),
	/* 2 */ H(1A1A), H(1B1E), H(1C1A), H(1D1E), H(1E1E), H(1F1A), H(2022), H(2126), H(2226), H(2322), H(2426), H(2522), H(2622), H(2726), H(282E), H(292A),
	/* 3 */ H(2A3A), H(2B3E), H(2C3A), H(2D3E), H(2E3E), H(2F3A), H(3026), H(3122), H(3222), H(3326), H(3422), H(3526), H(3626), H(3722), H(382A), H(392E),
	/* 4 */ H(3A3E), H(3B3A), H(3C3E), H(3D3A), H(3E3A), H(3F3E), H(4002), H(4106), H(4206), H(4302), H(4406), H(4502), H(4602), H(4706), H(480E), H(490A),
	/* 5 */ H(4A1A), H(4B1E), H(4C1A), H(4D1E), H(4E1E), H(4F1A), H(5006), H(5102), H(5202), H(5306), H(5402), H(5506), H(5606), H(5702), H(580A), H(590E),
	/* 6 */ H(5A1E), H(5B1A), H(5C1E), H(5D1A), H(5E1A), H(5F1E), H(6026), H(6122), H(6222), H(6326), H(6422), H(6526), H(6626), H(6722), H(682A), H(692E),
	/* 7 */ H(6A3E), H(6B3A), H(6C3E), H(6D3A), H(6E3A), H(6F3E), H(7022), H(7126), H(7226), H(7322), H(7426), H(7522), H(7622), H(7726), H(782E), H(792A),
	/* 8 */ H(7A3A), H(7B3E), H(7C3A), H(7D3E), H(7E3E), H(7F3A), H(8082), H(8186), H(8286), H(8382), H(8486), H(8582), H(8682), H(8786), H(888E), H(898A),
	/* 9 */ H(8A9A), H(8B9E), H(8C9A), H(8D9E), H(8E9E), H(8F9A), H(9086), H(9182), H(9282), H(9386), H(3423), H(3527), H(3627), H(3723), H(382B), H(392F),
	/* A */ H(3A3F), H(3B3B), H(3C3F), H(3D3B), H(3E3B), H(3F3F), H(4003), H(4107), H(4207), H(4303), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B),
	/* B */ H(4A1B), H(4B1F), H(4C1B), H(4D1F), H(4E1F), H(4F1B), H(5007), H(5103), H(5203), H(5307), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F),
	/* C */ H(5A1F), H(5B1B), H(5C1F), H(5D1B), H(5E1B), H(5F1F), H(6027), H(6123), H(6223), H(6327), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F),
	/* D */ H(6A3F), H(6B3B), H(6C3F), H(6D3B), H(6E3B), H(6F3F), H(7023), H(7127), H(7227), H(7323), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B),
	/* E */ H(7A3B), H(7B3F), H(7C3B), H(7D3F), H(7E3F), H(7F3B), H(8083), H(8187), H(8287), H(8383), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B),
	/* F */ H(8A9B), H(8B9F), H(8C9B), H(8D9F), H(8E9F), H(8F9B), H(9087), H(9183), H(9283), H(9387), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F),
	/* HNC */
	/* 111	0	 1	  2	   3	    4	     5	      6	       7	8	 9	  A	   B	    C	     D	      E	       F */
	/* 0 */ H(9A9F), H(9B9B), H(9C9F), H(9D9B), H(9E9B), H(9F9F), H(A0A7), H(A1A3), H(A2A3), H(A3A7), H(A4A3), H(A5A7), H(A6A7), H(A7A3), H(A8AB), H(A9AF),
	/* 1 */ H(AABF), H(ABBB), H(ACBF), H(ADBB), H(AEBB), H(AFBF), H(B0A3), H(B1A7), H(B2A7), H(B3A3), H(B4A7), H(B5A3), H(B6A3), H(B7A7), H(B8AF), H(B9AB),
	/* 2 */ H(BABB), H(BBBF), H(BCBB), H(BDBF), H(BEBF), H(BFBB), H(C087), H(C183), H(C283), H(C387), H(C483), H(C587), H(C687), H(C783), H(C88B), H(C98F),
	/* 3 */ H(CA9F), H(CB9B), H(CC9F), H(CD9B), H(CE9B), H(CF9F), H(D083), H(D187), H(D287), H(D383), H(D487), H(D583), H(D683), H(D787), H(D88F), H(D98B),
	/* 4 */ H(DA9B), H(DB9F), H(DC9B), H(DD9F), H(DE9F), H(DF9B), H(E0A3), H(E1A7), H(E2A7), H(E3A3), H(E4A7), H(E5A3), H(E6A3), H(E7A7), H(E8AF), H(E9AB),
	/* 5 */ H(EABB), H(EBBF), H(ECBB), H(EDBF), H(EEBF), H(EFBB), H(F0A7), H(F1A3), H(F2A3), H(F3A7), H(F4A3), H(F5A7), H(F6A7), H(F7A3), H(F8AB), H(F9AF),
	/* 6 */ H(FABF), H(FBBB), H(FCBF), H(FDBB), H(FEBB), H(FFBF), H(0047), H(0103), H(0203), H(0307), H(0403), H(0507), H(0607), H(0703), H(080B), H(090F),
	/* 7 */ H(0A1F), H(0B1B), H(0C1F), H(0D1B), H(0E1B), H(0F1F), H(1003), H(1107), H(1207), H(1303), H(1407), H(1503), H(1603), H(1707), H(180F), H(190B),
	/* 8 */ H(1A1B), H(1B1F), H(1C1B), H(1D1F), H(1E1F), H(1F1B), H(2023), H(2127), H(2227), H(2323), H(2427), H(2523), H(2623), H(2727), H(282F), H(292B),
	/* 9 */ H(2A3B), H(2B3F), H(2C3B), H(2D3F), H(2E3F), H(2F3B), H(3027), H(3123), H(3223), H(3327), H(3423), H(3527), H(3627), H(3723), H(382B), H(392F),
	/* A */ H(3A3F), H(3B3B), H(3C3F), H(3D3B), H(3E3B), H(3F3F), H(4003), H(4107), H(4207), H(4303), H(4407), H(4503), H(4603), H(4707), H(480F), H(490B),
	/* B */ H(4A1B), H(4B1F), H(4C1B), H(4D1F), H(4E1F), H(4F1B), H(5007), H(5103), H(5203), H(5307), H(5403), H(5507), H(5607), H(5703), H(580B), H(590F),
	/* C */ H(5A1F), H(5B1B), H(5C1F), H(5D1B), H(5E1B), H(5F1F), H(6027), H(6123), H(6223), H(6327), H(6423), H(6527), H(6627), H(6723), H(682B), H(692F),
	/* D */ H(6A3F), H(6B3B), H(6C3F), H(6D3B), H(6E3B), H(6F3F), H(7023), H(7127), H(7227), H(7323), H(7427), H(7523), H(7623), H(7727), H(782F), H(792B),
	/* E */ H(7A3B), H(7B3F), H(7C3B), H(7D3F), H(7E3F), H(7F3B), H(8083), H(8187), H(8287), H(8383), H(8487), H(8583), H(8683), H(8787), H(888F), H(898B),
	/* F */ H(8A9B), H(8B9F), H(8C9B), H(8D9F), H(8E9F), H(8F9B), H(9087), H(9183), H(9283), H(9387), H(9483), H(9587), H(9687), H(9783), H(988B), H(998F)};

#	undef H
#endif


/* MARK: - Types */

typedef zuint8 (* Insn)(Z80 *self);

#ifdef Z80_WITH_FULL_IM0
	typedef struct {
		Z80*	  z80;
		void*	  context;
		Z80Read	  fetch;
		Z80Read	  read;
		Z80Write  write;
		Z80Read	  in;
		Z80Write  out;
		Z80Notify ld_i_a;
		Z80Notify ld_r_a;
		Z80Notify reti;
		Z80Notify retn;
		zuint16	  pc;
	} IM0;
#endif


/* MARK: - Instance Variable and Callback Shortcuts */

#define MEMPTR	  self->memptr.uint16_value
#define PC	  self->pc.uint16_value
#define SP	  self->sp.uint16_value
#define XY	  self->xy.uint16_value
#define IX	  self->ix_iy[0].uint16_value
#define IY	  self->ix_iy[1].uint16_value
#define AF	  self->af.uint16_value
#define BC	  self->bc.uint16_value
#define DE	  self->de.uint16_value
#define HL	  self->hl.uint16_value
#define AF_	  self->af_.uint16_value
#define BC_	  self->bc_.uint16_value
#define DE_	  self->de_.uint16_value
#define HL_	  self->hl_.uint16_value
#define MEMPTRH	  self->memptr.uint8_values.at_1
#define MEMPTRL	  self->memptr.uint8_values.at_0
#define PCH	  self->pc.uint8_values.at_1
#define A	  self->af.uint8_values.at_1
#define F	  self->af.uint8_values.at_0
#define B	  self->bc.uint8_values.at_1
#define C	  self->bc.uint8_values.at_0
#define E	  self->de.uint8_values.at_0
#define L	  self->hl.uint8_values.at_0
#define I	  self->i
#define R	  self->r
#define R7	  self->r7
#define Q	  self->q
#define IFF1	  self->iff1
#define IFF2	  self->iff2
#define IM	  self->im
#define HALT_LINE self->halt_line
#define INT_LINE  self->int_line
#define DATA	  self->data.uint8_array
#define REQUEST	  self->request
#define RESUME	  self->resume
#define OPTIONS	  self->options
#define CONTEXT	  self->context

#define FETCH_OPCODE(address) self->fetch_opcode(CONTEXT, address)
#define FETCH(address)	      self->fetch	(CONTEXT, address)
#define READ(address)	      self->read	(CONTEXT, address)
#define WRITE(address, value) self->write	(CONTEXT, address, value)
#define IN(port)	      self->in		(CONTEXT, port)
#define OUT(port, value)      self->out		(CONTEXT, port, value)
#define NOTIFY(callback)      if (self->callback != Z_NULL) self->callback(CONTEXT)


/* MARK: - 16-bit Callback Operations */

static Z_ALWAYS_INLINE zuint16 fetch_16(Z80 *self, zuint16 address)
	{
	zuint8 l = FETCH(address);
	return (zuint16)(l | ((zuint16)FETCH(address + 1) << 8));
	}


static Z_ALWAYS_INLINE zuint16 read_16(Z80 *self, zuint16 address)
	{
	zuint8 l = READ(address);
	return (zuint16)(l | ((zuint16)READ(address + 1) << 8));
	}


static Z_ALWAYS_INLINE void write_16f(Z80 *self, zuint16 address, zuint16 value)
	{
	WRITE(address, (zuint8)value);
	WRITE(address + 1, (zuint8)(value >> 8));
	}


static Z_ALWAYS_INLINE void write_16b(Z80 *self, zuint16 address, zuint16 value)
	{
	WRITE(address + 1, (zuint8)(value >> 8));
	WRITE(address, (zuint8)value);
	}


#ifndef Z80_WITH_FULL_IM0
	static Z_ALWAYS_INLINE zuint16 int_fetch_16(Z80 *self)
		{
		zuint8 l = self->int_fetch(CONTEXT, PC);
		return (zuint16)(l | ((zuint16)self->int_fetch(CONTEXT, PC) << 8));
		}
#endif


#define FETCH_16(address)	  fetch_16 (self, address)
#define READ_16(address)	  read_16  (self, address)
#define WRITE_16F(address, value) write_16f(self, address, value)
#define WRITE_16B(address, value) write_16b(self, address, value)


/* MARK: - Interrupt Mode 0: Callback Trampolines */

#ifdef Z80_WITH_FULL_IM0
	static zuint8 im0_fetch(IM0 const *self, zuint16 address)
		{
		Z_UNUSED(address)
		return self->z80->int_fetch(CONTEXT, self->pc);
		}


	static zuint8 im0_read(IM0 const *self, zuint16 address)
		{return READ(address);}


	static void im0_write(IM0 const *self, zuint16 address, zuint8 value)
		{WRITE(address, value);}


	static zuint8 im0_in(IM0 const *self, zuint16 port)
		{return IN(port);}


	static void im0_out(IM0 const *self, zuint16 port, zuint8 value)
		{OUT(port, value);}


	static void im0_ld_i_a(IM0 const *self) {NOTIFY(ld_i_a);}
	static void im0_ld_r_a(IM0 const *self) {NOTIFY(ld_r_a);}


#	ifdef Z80_WITH_IM0_RETX_NOTIFICATIONS
		#define IM0_NOTIFY_RETX(callback)		     \
			if (	self->callback != Z_NULL &&	     \
				(self->z80->options &		     \
				Z80_OPTION_IM0_RETX_NOTIFICATIONS)   \
			)					     \
				{				     \
				self->z80->data.uint8_array[2] |= 2; \
				self->callback(CONTEXT);	     \
				}


		static void im0_reti(IM0 const *self) {IM0_NOTIFY_RETX(reti)}
		static void im0_retn(IM0 const *self) {IM0_NOTIFY_RETX(retn)}
#	endif
#endif


/* MARK: - Interrupt Mode 0: PC Decrements for Unprefixed Instructions */

#ifdef Z80_WITH_FULL_IM0
	static zuint8 const im0_pc_decrement_table[256] = {
	/*	0  1  2  3  4  5  6  7	8  9  A  B  C  D  E  F */
	/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 1 */ 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
	/* 2 */ 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
	/* 3 */ 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
	/* 4 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 5 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 6 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 7 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* C */ 1, 0, 3, 3, 3, 0, 0, 1, 1, 1, 3, 0, 3, 3, 0, 1,
	/* D */ 1, 0, 3, 0, 3, 0, 0, 1, 1, 0, 3, 0, 3, 0, 0, 1,
	/* E */ 1, 0, 3, 0, 3, 0, 0, 1, 1, 1, 3, 0, 3, 0, 0, 1,
	/* F */ 1, 0, 3, 0, 3, 0, 0, 1, 1, 0, 3, 0, 3, 0, 0, 1};
#endif


/* MARK: - Flags */

/*----------------.
| 7 6 5 4 3 2 1 0 |
| S Z Y H X P N C |
'-|-|-|-|-|-|-|-|-'
  | | | | | | | '-- carry / borrow
  | | | | | | '---- addition / subtraction
  | | | | | '------ parity (P) / two's complement signed overflow (V)
  | | | | '-------- result's bit 3 (undocumented)
  | | | '---------- half carry / half borrow
  | | '------------ result's bit 5 (undocumented)
  | '-------------- zero
  '---------------- sign */

#define SF 128
#define ZF  64
#define YF  32
#define HF  16
#define XF   8
#define PF   4
#define NF   2
#define CF   1

#define SZPCF (SF | ZF | PF | CF)
#define SZPF  (SF | ZF | PF	)
#define SZCF  (SF | ZF | CF	)
#define SYXF  (SF | YF | XF	)
#define ZPF   (ZF | PF		)
#define YXCF  (YF | XF | CF	)
#define YXF   (YF | XF		)
#define HCF   (HF | CF		)

#define F_SZPC (F & SZPCF)
#define F_SZP  (F &  SZPF)
#define F_SZC  (F &  SZCF)
#define F_C    (F &    CF)
#define A_SYX  (A &  SYXF)
#define A_YX   (A &   YXF)

#define ZF_ZERO(value) (!(value) << 6)

#ifdef Z80_WITH_PARITY_COMPUTATION
	static Z_ALWAYS_INLINE zuint8 pf_parity(zuint8 value)
		{return (zuint8)(((0x9669U >> ((value ^ (value >> 4)) & 0xF)) & 1) << 2);}

#	define PF_PARITY pf_parity
#else
	static zuint8 const pf_parity_table[256] = {
	/*	0  1  2  3  4  5  6  7	8  9  A  B  C  D  E  F */
	/* 0 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* 1 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* 2 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* 3 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* 4 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* 5 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* 6 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* 7 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* 8 */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* 9 */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* A */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* B */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* C */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
	/* D */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* E */ 0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
	/* F */ 4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4};

#	define PF_PARITY(value) pf_parity_table[value]
#endif

/*-----------------------------------------------------------------------------.
| `PF_OVERFLOW` computes PF according to whether signed overflow occurs in the |
| addition or subtraction of two integers. For additions, `rhs` must be passed |
| bit-wise ~inverted. For an explanation of the formula, check:		       |
| * https://stackoverflow.com/a/199668					       |
| * http://www.cs.umd.edu/class/spring2003/cmsc311/Notes/Comb/overflow.html    |
'=============================================================================*/

#define PF_OVERFLOW(width, result, lhs, rhs) \
	(((zuint##width)((lhs ^ rhs) & (lhs ^ result)) >> (width - 3)) & PF)

/*-----------------------------------------------------------------------------.
| Q serves as an abstraction for a set of latches related to flag computation. |
| From an emulation perspective, instructions that affect the flags copy the   |
| final value of F to Q, whereas instructions that do not affect the flags     |
| (including `ex af,af'`, `pop af`, internal NOPs and interrupt responses) set |
| Q to 0. Q is used to compute YF and XF in the `ccf` and `scf` instructions.  |
|									       |
| References:								       |
| * https://worldofspectrum.org/forums/discussion/20345			       |
| * https://worldofspectrum.org/forums/discussion/41704			       |
'=============================================================================*/

#ifdef Z80_WITH_Q
#	define FLAGS Q = F
#	define Q_0   Q = 0;
#else
#	define FLAGS F
#	define Q_0
#endif


/* MARK: - Bit Rotation */

#define ROL(value) Z_UINT8_ROTATE_LEFT (value, 1)
#define ROR(value) Z_UINT8_ROTATE_RIGHT(value, 1)


/* MARK: - 8-Bit Register Resolution */

/*---------.   .---------------------.
| 76543210 |   |  J / K  |   O / P   |
|----------|   |---------+-----------|
| __jjj___ |   | 000 = b | 000 = b   |
| _____kkk |   | 001 = c | 001 = c   |
| __ooo___ |   | 010 = d | 010 = d   |
| _____ppp |   | 011 = e | 011 = e   |
'----------'   | 100 = h | 100 = XYh |
	       | 101 = l | 101 = XYl |
	       | 111 = a | 111 = a   |
	       '--------------------*/

static zusize const j_k_table[8] = {
	Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_0),
	Z_MEMBER_OFFSET(Z80, de.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, de.uint8_values.at_0),
	Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, hl.uint8_values.at_0),
	0,
	Z_MEMBER_OFFSET(Z80, af.uint8_values.at_1)};

static zusize const o_p_table[8] = {
	Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, bc.uint8_values.at_0),
	Z_MEMBER_OFFSET(Z80, de.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, de.uint8_values.at_0),
	Z_MEMBER_OFFSET(Z80, xy.uint8_values.at_1),
	Z_MEMBER_OFFSET(Z80, xy.uint8_values.at_0),
	0,
	Z_MEMBER_OFFSET(Z80, af.uint8_values.at_1)};

#define REGISTER_8(table, offset, shift) \
	*((zuint8 *)self + table[(DATA[offset] shift) & 7])

#define J0 REGISTER_8(j_k_table, 0, >> 3   )
#define J1 REGISTER_8(j_k_table, 1, >> 3   )
#define K0 REGISTER_8(j_k_table, 0, Z_EMPTY)
#define K1 REGISTER_8(j_k_table, 1, Z_EMPTY)
#define K3 REGISTER_8(j_k_table, 3, Z_EMPTY)
#define O  REGISTER_8(o_p_table, 1, >> 3   )
#define P  REGISTER_8(o_p_table, 1, Z_EMPTY)


/* MARK: - 16-Bit Register Resolution */

/*---------.   .-----------------------------.
| 76543210 |   |    S	 |    T    |	W    |
|----------|   |---------+---------+---------|
| __ss____ |   | 00 = bc | 00 = bc | 00 = bc |
| __tt____ |   | 01 = de | 01 = de | 01 = de |
| __ww____ |   | 10 = hl | 10 = hl | 10 = XY |
'----------'   | 11 = sp | 11 = af | 11 = sp |
	       '----------------------------*/

static zusize const s_table[4] = {
	Z_MEMBER_OFFSET(Z80, bc.uint16_value),
	Z_MEMBER_OFFSET(Z80, de.uint16_value),
	Z_MEMBER_OFFSET(Z80, hl.uint16_value),
	Z_MEMBER_OFFSET(Z80, sp.uint16_value)};

static zusize const t_table[4] = {
	Z_MEMBER_OFFSET(Z80, bc.uint16_value),
	Z_MEMBER_OFFSET(Z80, de.uint16_value),
	Z_MEMBER_OFFSET(Z80, hl.uint16_value),
	Z_MEMBER_OFFSET(Z80, af.uint16_value)};

static zusize const w_table[4] = {
	Z_MEMBER_OFFSET(Z80, bc.uint16_value),
	Z_MEMBER_OFFSET(Z80, de.uint16_value),
	Z_MEMBER_OFFSET(Z80, xy.uint16_value),
	Z_MEMBER_OFFSET(Z80, sp.uint16_value)};

#define REGISTER_16(table, offset) \
	*(zuint16 *)(void *)((zchar *)self + table[(DATA[offset] >> 4) & 3])

#define SS0 REGISTER_16(s_table, 0)
#define SS1 REGISTER_16(s_table, 1)
#define TT  REGISTER_16(t_table, 0)
#define WW  REGISTER_16(w_table, 1)


/* MARK: - Condition Evaluation */

/*---------.   .----------.
| 76543210 |   |     Z	  |
|----------|   |----------|
| __zzz___ |   | 000 = nz |
| ___zz___ |   | 001 = z  |
'----------'   | 010 = nc |
	       | 011 = c  |
	       | 100 = po |
	       | 101 = pe |
	       | 110 = p  |
	       | 111 = m  |
	       '---------*/

static zuint8 const z_table[8] = {ZF, ZF, CF, CF, PF, PF, SF, SF};


static Z_ALWAYS_INLINE zsint zzz(Z80 const *self, zuint8 mask)
	{
	zsint z = (DATA[0] >> 3) & mask;

	return !(F & z_table[z]) ^ (z & 1);
	}


/* MARK: - 8-Bit Arithmetic and Logical Operations */

/*---------.   .---------------------.
| 76543210 |   |     U	    SZYHXPNC |
|----------|   |---------------------|
| __uuu___ |   | 000 = add  szycxv0c |
| _____10v |   | 001 = adc  szycxv0c |
'----------'   | 010 = sub  szybxv1b |
	       | 011 = sbc  szybxv1b |
	       | 100 = and  szy1xp00 |
	       | 101 = xor  szy0xp00 |
	       | 110 = or   szy0xp00 |
	       | 111 = cp   sz*b*v1b |
	       |---------------------|
	       |     V	    SZYHXPNC |
	       |---------------------|
	       | 100 = inc  szycxv0. |
	       | 101 = dec  szybxv1. |
	       '--------------------*/

static void uuu(Z80 *self, zuint8 offset, zuint8 rhs)
	{
	zuint8 t, f;

	switch ((DATA[offset] >> 3) & 7)
		{
		case 0: /* add */
		t = A + rhs;

		f =	((zuint)A + rhs > 255)     | /* CF = carry	*/
			PF_OVERFLOW(8, t, A, ~rhs) | /* PF = overflow	*/
			((A ^ rhs ^ t) & HF);	     /* HF = half-carry */
						     /* NF = 0		*/
		A = t;
		break;

		case 1: /* adc */
		t = A + rhs + (f = F_C);

		f =	((zuint)A + rhs + f > 255) | /* CF = carry	*/
			PF_OVERFLOW(8, t, A, ~rhs) | /* PF = overflow	*/
			((A ^ rhs ^ t) & HF);	     /* HF = half-carry */
						     /* NF = 0		*/
		A = t;
		break;

		case 2: /* sub */
		t = A - rhs;

		f =	(A < rhs)		  | /* CF = borrow	*/
			NF			  | /* NF = 1		*/
			PF_OVERFLOW(8, t, A, rhs) | /* PF = overflow	*/
			((A ^ rhs ^ t) & HF);	    /* HF = half-borrow */

		A = t;
		break;

		case 3: /* sbc */
		t = A - rhs - (f = F_C);

		f =	((zsint)A - rhs - f < 0)  | /* CF = borrow	*/
			NF			  | /* NF = 1		*/
			PF_OVERFLOW(8, t, A, rhs) | /* PF = overflow	*/
			((A ^ rhs ^ t) & HF);	    /* HF = half-borrow */

		A = t;
		break;

		case 4: /* and */
		A &= rhs;
		f = HF | PF_PARITY(A); /* HF = 1; PF = parity */
		break;		       /* NF, CF = 0	      */

		case 5: /* xor */
		A ^= rhs;
		f = PF_PARITY(A); /* PF = parity    */
		break;		  /* HF, NF, CF = 0 */

		case 6: /* or */
		A |= rhs;
		f = PF_PARITY(A); /* PF = parity    */
		break;		  /* HF, NF, CF = 0 */

		case 7: /* cp */
		t = A - rhs;

		FLAGS = (zuint8)(
			(t & SF)		  | /* SF = sign	      */
			ZF_ZERO(t)		  | /* ZF = zero	      */
			((A ^ rhs ^ t) & HF)	  | /* HF = half-borrow	      */
			PF_OVERFLOW(8, t, A, rhs) | /* PF = overflow	      */
			(A < rhs)		  | /* CF = borrow	      */
			(rhs & YXF)		  | /* YF = rhs.5; XF = rhs.3 */
			NF);			    /* NF = 1		      */

		return;
		}

	FLAGS = (zuint8)(
		f     |	     /* HF, PF, NF and CF already computed */
		A_SYX |	     /* SF = sign; YF = Y; XF = X	   */
		ZF_ZERO(A)); /* ZF = zero			   */
	}


static zuint8 vvv(Z80 *self, zuint8 offset, zuint8 value)
	{
	zuint8 dec = DATA[offset] & 1;
	zuint8 nf  = (zuint8)(dec << 1);
	zuint8 t   = value + 1 - nf;

	FLAGS = (zuint8)(
		(t & SYXF)		    | /* SF = sign; YF = Y; XF = X */
		ZF_ZERO(t)		    | /* ZF = zero		   */
		((value ^ t) & HF)	    | /* HF = half-carry/borrow	   */
		((value == 127 + dec) << 2) | /* PF = overflow		   */
		nf			    | /* NF = 0 (inc), 1 (dec)	   */
		F_C);			      /* CF unchanged		   */

	return t;
	}


/* MARK: - Rotation and Shift Operations */

/*---------.   .-----------.
| 76543210 |   |     G	   |
|----------|   |-----------|
| __ggg___ |   | 000 = rlc |
'----------'   | 001 = rrc |
	       | 010 = rl  |
	       | 011 = rr  |
	       | 100 = sla |
	       | 101 = sra |
	       | 110 = sll |
	       | 111 = srl |
	       '----------*/

static zuint8 ggg(Z80 *self, zuint8 offset, zuint8 value)
	{
	zuint8 cf;

	switch ((DATA[offset] >> 3) & 7)
		{
		/* rlc		 .----------------.
			.----.	 |  .---------.   |
			| CF |<-----| 7 <-- 0 |<--'
			'----'	    '--------*/
		case 0:
		cf = (value = ROL(value)) & 1;
		break;

		/* rrc	.----------------.
			|   .---------.  |   .----.
			'-->| 7 --> 0 |----->| CF |
			    '---------'      '---*/
		case 1:
		cf = value & 1;
		value = ROR(value);
		break;

		/* rl	.-------------------------.
			|  .----.   .---------.   |
			'--| CF |<--| 7 <-- 0 |<--'
			   '----'   '--------*/
		case 2:
		cf = value >> 7;
		value = (zuint8)((value << 1) | F_C);
		break;

		/* rr	.-------------------------.
			|   .---------.   .----.  |
			'-->| 7 --> 0 |-->| CF |--'
			    '---------'   '---*/
		case 3:
		cf = value & 1;
		value = (zuint8)((value >> 1) | ((zuint8)F_C << 7));
		break;

		/* sla	.----.	 .---------.
			| CF |<--| 7 <-- 0 |<-- 0
			'----'	 '--------*/
		case 4:
		cf = value >> 7;
		value <<= 1;
		break;

		/* sra	    .---------.   .----.
			.-->| 7 --> 0 |-->| CF |
			|   '-|-------'   '----'
			'----*/
		case 5:
		cf = value & 1;
		value = (zuint8)((value & 128) | (value >> 1));
		break;

		/* sll	.----.	 .---------.
			| CF |<--| 7 <-- 0 |<-- 1
			'----'	 '--------*/
		case 6:
		cf = value >> 7;
		value = (zuint8)((value << 1) | 1);
		break;

		/* srl	     .---------.   .----.
			0 -->| 7 --> 0 |-->| CF |
			     '---------'   '---*/
		case 7:
		cf = value & 1;
		value >>= 1;
		break;

		/* Uncoment to avoid compiler warnings */
		/*default: cf = 0;*/
		}

	FLAGS = (zuint8)(	   /* HF, NF = 0		*/
		(value & SYXF)	 | /* SF = sign; YF = Y; XF = X */
		ZF_ZERO(value)	 | /* ZF = zero			*/
		PF_PARITY(value) | /* PF = parity		*/
		cf);		   /* CF already computed	*/

	return value;
	}


/* MARK: - Bit Set and Reset Operations */

/*---------.   .---------.
| 76543210 |   |    M	 |
|----------|   |---------|
| _m______ |   | 0 = res |
'----------'   | 1 = set |
	       '--------*/

static Z_ALWAYS_INLINE zuint8 m(Z80 *self, zuint8 offset, zuint8 value)
	{
	zuint8 t;

	Q_0

	return (zuint8)(((t = DATA[offset]) & 64)
		? value |  (1U << ((t >> 3) & 7))
		: value & ~(1U << ((t >> 3) & 7)));
	}


/* MARK: - Function Shortcuts and Reusable Code */

#define INSN(name)	     static zuint8 name(Z80 *self)
#define N(offset)	     ((DATA[offset] >> 3) & 7)
#define Z(mask)		     zzz(self, mask)
#define U0(value)	     uuu(self, 0, value)
#define U1(value)	     uuu(self, 1, value)
#define V0(value)	     vvv(self, 0, value)
#define V1(value)	     vvv(self, 1, value)
#define G1(value)	     ggg(self, 1, value)
#define G3(value)	     ggg(self, 3, value)
#define M1(value)	     m	(self, 1, value)
#define M3(value)	     m	(self, 3, value)
#define PUSH(value)	     WRITE_16B(SP -= 2, value)
#define R_ALL		     ((R & 127) | (R7 & 128))
#define RET		     MEMPTR = PC = READ_16(SP); SP += 2
#define FETCH_XY_EA(address) MEMPTR = (zuint16)(XY + (zsint8)FETCH(address))
#define IS_XY_PREFIX(opcode) ((opcode) & 0xDF) == 0xDD
#define EX(a, b)	     t = a; a = b; b = t


#define LD_A_IR(rhs)					      \
	A = rhs;					      \
							      \
	FLAGS = (zuint8)(     /* HF, NF = 0		   */ \
		A_SYX	    | /* SF = sign; YF = Y; XF = X */ \
		ZF_ZERO(A)  | /* ZF = zero		   */ \
		(IFF2 << 2) | /* PF = IFF2		   */ \
		F_C);	      /* CF unchanged		   */ \
							      \
	PC += 2;					      \
	return 9


#define LD_VWORD_COMMON(insn_size)			   \
	zuint16 n;					   \
							   \
	Q_0						   \
	MEMPTR = (n = FETCH_16((PC += insn_size) - 2)) + 1


#define EX_VSP(rhs, pc_increment)	 \
	zuint16 sp, t = rhs;		 \
					 \
	Q_0				 \
	pc_increment;			 \
	rhs = MEMPTR = READ_16(sp = SP); \
	WRITE_16B(sp, t);		 \
	return 19


#define LDX(operator)					     \
	zuint8 t = READ(HL operator);			     \
							     \
	WRITE(DE operator, t);				     \
	t += A;						     \
							     \
	FLAGS = (zuint8)(	  /* HF, NF = 0		  */ \
		F_SZC	       |  /* SF, ZF, CF unchanged */ \
		((t & 2) << 4) |  /* YF = (A + [HLi]).1	  */ \
		(t & XF)       |  /* XF = (A + [HLi]).3	  */ \
		(!!(--BC) << 2)); /* PF = !!BCo		  */ \
							     \
	PC += 2;					     \
	return 16


#define LDXR(operator)							   \
	zuint8 t = READ(HL operator);					   \
									   \
	WRITE(DE operator, t);						   \
	t += A;								   \
									   \
	if (--BC)							   \
		{			    /* HF, NF = 0		*/ \
		FLAGS = F_SZC		  | /* SF, ZF, CF unchanged	*/ \
			((PC >> 8) & YXF) | /* YF = PCi.13; XF = PCi.11 */ \
			PF;		    /* PF = 1			*/ \
									   \
		MEMPTR = PC + 1;					   \
		return 21;						   \
		}							   \
									   \
	FLAGS = (zuint8)(	 /* HF, PF, NF = 0	 */		   \
		F_SZC	       | /* SF, ZF, CF unchanged */		   \
		((t & 2) << 4) | /* YF = (A + [HLi]).1	 */		   \
		(t & XF));	 /* XF = (A + [HLi]).3	 */		   \
									   \
	PC += 2;							   \
	return 16


#define CPX(operator)						 \
	zuint8 n  = READ(HL operator);				 \
	zuint8 t0 = A - n;					 \
	zuint8 hf = (A ^ n ^ t0) & HF;				 \
	zuint8 t1 = t0 - (hf >> 4);				 \
								 \
	FLAGS = (zuint8)(					 \
		(t0 & SF)	| /* SF = sign		      */ \
		ZF_ZERO(t0)	| /* ZF = zero		      */ \
		hf		| /* HF = half-borrow	      */ \
		((t1 & 2) << 4) | /* YF = (A - [HLi] - HFo).1 */ \
		(t1 & XF)	| /* XF = (A - [HLi] - HFo).3 */ \
		(!!(--BC) << 2) | /* PF = !!BCo		      */ \
		NF		| /* NF = 1		      */ \
		F_C);		  /* CF unchanged	      */ \
								 \
	MEMPTR operator;					 \
	PC += 2;						 \
	return 16


#define CPXR(operator)						 \
	zuint8 n  = READ(HL operator);				 \
	zuint8 t0 = A - n;					 \
	zuint8 hf = (A ^ n ^ t0) & HF;				 \
	zuint8 t1 = t0 - (hf >> 4);				 \
								 \
	zuint8 f = (zuint8)(					 \
		(t0 & SF)	| /* SF = sign	      */	 \
		ZF_ZERO(t0)	| /* ZF = zero	      */	 \
		hf		| /* HF = half-borrow */	 \
		(!!(--BC) << 2) | /* PF = !!BCo	      */	 \
		NF		| /* NF = 1	      */	 \
		F_C);		  /* CF unchanged     */	 \
								 \
	if (t0 && BC)						 \
		{						 \
		/* YF = PCi.13; XF = PCi.11 */			 \
		FLAGS = f | ((PC >> 8) & YXF);			 \
		MEMPTR = PC + 1;				 \
		return 21;					 \
		}						 \
								 \
	FLAGS = (zuint8)(					 \
		f		|				 \
		((t1 & 2) << 4) | /* YF = (A - [HLi] - HFo).1 */ \
		(t1 & XF));	  /* XF = (A - [HLi] - HFo).3 */ \
								 \
	MEMPTR operator;					 \
	PC += 2;						 \
	return 16


#define ADD_16(lhs, rhs, pc_increment)					     \
	zuint16 n = rhs;						     \
	zuint32 t = lhs + n;						     \
									     \
	FLAGS = (zuint8)(		      /* NF = 0			  */ \
		F_SZP			    | /* SF, ZF, PF unchanged	  */ \
		((t >> 8) & YXF)	    | /* YF = high-Y; XF = high-X */ \
		(((lhs ^ n ^ t) >> 8) & HF) | /* HF = high-half-carry	  */ \
		((t >> 16) & 1));	      /* CF = carry		  */ \
									     \
	MEMPTR = lhs + 1;						     \
	lhs = (zuint16)t;						     \
	pc_increment;							     \
	return 11


#define ADC_SBC_HL_SS(operator, pf_overflow_rhs, or_nf)			    \
	zuint16 ss = SS1;						    \
	zuint32 t  = HL operator ss operator F_C;			    \
									    \
	FLAGS = (zuint8)(						    \
		((t >> 8) & SYXF) /* SF = sign; YF = high-Y; XF = high-X */ \
		| (zuint8)ZF_ZERO((zuint16)t) /* ZF = zero		 */ \
		/* HF = high-half-carry (adc), high-half-borrow (sbc)	 */ \
		| (((HL ^ ss ^ t) >> 8) & HF)				    \
		/* PF = overflow					 */ \
		| PF_OVERFLOW(16, t, HL, pf_overflow_rhs)		    \
		| ((t >> 16) & 1) /* CF = carry (adc), borrow (sbc)	 */ \
		or_nf);		  /* NF = 0 (adc), 1 (sbc)		 */ \
									    \
	MEMPTR = HL + 1;						    \
	HL = (zuint16)t;						    \
	PC += 2;							    \
	return 15


/* rla	.-------------------------.   rra  .-------------------------.
	|  .----.   .----A----.	  |	   |   .----A----.   .----.  |
	'--| CF |<--| 7 <-- 0 |<--'	   '-->| 7 --> 0 |-->| CF |--'
	   '----'   '---------'		       '---------'   '---*/

#define RXA(a_to_cf, operator, fc_to_a)			  \
	zuint8 cf = a_to_cf;				  \
							  \
	A = (zuint8)((A operator 1) | fc_to_a);		  \
							  \
	FLAGS = F_SZP | /* SF, ZF, PF unchanged	       */ \
		A_YX  | /* YF = Y; XF = X	       */ \
		cf;	/* CF = Ai.7 (rla), Ai.0 (rra) */ \
			/* HF, NF = 0		       */ \
	PC++;						  \
	return 4


/* rld	 .------------------------.	 rrd   .------------------------.
	 |			  |	       |			|
.--------|--.	.-------------.	  |   .--------V--.   .-------------.	|
| 7-4 | 3-0 |<--| 7-4 <-- 3-0 |<--'   | 7-4 | 3-0 |-->| 7-4 --> 3-0 |---'
'-----A-----'	'----(HL)-----'	      '-----A-----'   '----(HL)----*/

#define RXD(vhl_to_vhl, a_to_vhl, vhl_to_a)		       \
	zuint8 t = READ(HL);				       \
							       \
	MEMPTR = HL + 1;				       \
	WRITE(HL, (zuint8)((t vhl_to_vhl) | (A a_to_vhl)));    \
	A = (A & 0xF0) | (t vhl_to_a);			       \
							       \
	FLAGS = (zuint8)(      /* HF, NF = 0		    */ \
		A_SYX	     | /* SF = sign; YF = Y; XF = X */ \
		ZF_ZERO(A)   | /* ZF = zero		    */ \
		PF_PARITY(A) | /* PF = parity		    */ \
		F_C);	       /* CF unchanged		    */ \
							       \
	PC += 2;					       \
	return 18


#define DJNZ_JR_Z(condition, cycles_if_true, cycles_if_false) \
	zsint8 offset;					      \
							      \
	Q_0						      \
	offset = (zsint8)FETCH(PC + 1); /* Always read */     \
							      \
	if (condition)					      \
		{					      \
		MEMPTR = (PC += 2 + offset);		      \
		return cycles_if_true;			      \
		}					      \
							      \
	PC += 2;					      \
	return cycles_if_false


#define RETX(mnemonic)						   \
	DATA[2] = IFF1;						   \
	NOTIFY(mnemonic);					   \
	Q_0							   \
	RET;							   \
	if ((IFF1 = IFF2) && INT_LINE) REQUEST |= Z80_REQUEST_INT; \
	return 14


#define IN_VC(set_lhs)					       \
	zuint8 t;					       \
							       \
	MEMPTR = BC + 1;				       \
	t = IN(BC);					       \
							       \
	FLAGS = (zuint8)(      /* HF, NF = 0		    */ \
		(t & SYXF)   | /* SF = sign; YF = Y; XF = X */ \
		ZF_ZERO(t)   | /* ZF = zero		    */ \
		PF_PARITY(t) | /* PF = parity		    */ \
		F_C);	       /* CF unchanged		    */ \
							       \
	set_lhs						       \
	PC += 2;					       \
	return 12


#define INX_OUTX_COMMON(io)						       \
	FLAGS = (zuint8)(						       \
		(B & SYXF)	       | /* SF = Bo.7; YF = Bo.5; XF = Bo.3 */ \
		ZF_ZERO(B)	       | /* ZF = !Bo			    */ \
		PF_PARITY((t & 7) ^ B) | /* PF = ((T & 7) ^ Bo).parity	    */ \
		((t > 255) ? HCF : 0)  | /* HF, CF = T > 255		    */ \
		((io >> 6) & NF));	 /* NF = IO.7			    */ \
									       \
	PC += 2;							       \
	return 16


#define INX(hl_operator, memptr_operator)		       \
	zuint8 io = IN(BC);				       \
	zuint  t  = (zuint)io + (zuint8)(C memptr_operator 1); \
							       \
	WRITE(HL hl_operator, io);			       \
	MEMPTR = BC memptr_operator 1;			       \
	B--;						       \
	INX_OUTX_COMMON(io)


#define OUTX(hl_operator, memptr_operator) \
	zuint8 io = READ(HL hl_operator);  \
	zuint  t  = (zuint)io + L;	   \
					   \
	B--;				   \
	MEMPTR = BC memptr_operator 1;	   \
	OUT(BC, io);			   \
	INX_OUTX_COMMON(io)


/*-----------------------------------------------------------------------------.
| Block instructions produce an extra M-cycle of 5 T-states to decrement PC if |
| the loop condition is met. In 2018, David Banks (AKA hoglet) discovered that |
| the Z80 CPU performs additional flag changes during this M-cycle and managed |
| to decipher the behaviors. All block instructions copy bits 13 and 11 of PCi |
| to YF and XF, respectively [1.1], but `inir`, `indr`, `otir` and `otdr` also |
| modify HF and PF in a very complicated way [1.2]. These latter two flags are |
| not commented here because the explanation would not be simpler than the     |
| code itself, so please refer to David Banks' paper [2] for more information. |
|									       |
| David Banks' discoveries have been corroborated thanks to Peter Helcmanovsky |
| (AKA Ped7g), who wrote a test that covers most of the cases that can be      |
| verified on a ZX Spectrum [3].					       |
|									       |
| In 2022, rofl0r discovered that the instructions `otir` and `otdr` also set  |
| MEMPTR to `PCi + 1` during the extra M-cycle [4]. However, this information  |
| was not announced anywhere and went unnoticed by the emulation community     |
| until 2023, when Manuel Sainz de Baranda y Goñi rediscovered the same	       |
| behaviour in all four I/O block instructions: `inir`, `indr`, `otir` and     |
| `otdr` [5].								       |
|									       |
| References:								       |
| 1. https://stardot.org.uk/forums/viewtopic.php?t=15464		       |
|     1. https://stardot.org.uk/forums/viewtopic.php?p=211042#p211042	       |
|     2. https://stardot.org.uk/forums/viewtopic.php?p=212021#p212021	       |
| 2. Banks, David (2018-08-21). "Undocumented Z80 Flags" rev. 1.0.	       |
|     * https://github.com/hoglet67/Z80Decoder/wiki/Undocumented-Flags	       |
|     * https://stardot.org.uk/forums/download/file.php?id=39831	       |
| 3. Helcmanovsky, Peter (2021/2022). "Z80 Block Flags Test".		       |
|     * https://github.com/MrKWatkins/ZXSpectrumNextTests		       |
| 4. https://github.com/hoglet67/Z80Decoder/issues/2			       |
| 5. https://spectrumcomputing.co.uk/forums/viewtopic.php?t=10555	       |
'=============================================================================*/

#define INXR_OTXR_COMMON					      \
	if (B)	{						      \
		FLAGS = (zuint8)(     /* ZF = 0			  */  \
			(B & SF)    | /* SF = Bo.7		  */  \
			(PCH & YXF) | /* YF = PCi.13; XF = PCi.11 */  \
			nf	    | /* NF = IO.7		  */  \
			(hcf	?     /* CF = T > 255		  */  \
				CF |				      \
				(nf	?			      \
					(!(B & 0xF) << 4) |	      \
					PF_PARITY(p ^ ((B - 1) & 7))  \
					:			      \
					(((B & 0xF) == 0xF) << 4) |   \
					PF_PARITY(p ^ ((B + 1) & 7))) \
				: PF_PARITY(p ^ (B & 7))));	      \
								      \
		MEMPTR = PC + 1;				      \
		return 21;					      \
		}						      \
								      \
	FLAGS = ZF	     | /* ZF = 1; SF, YF, XF = 0     */	      \
		hcf	     | /* HF, CF = T > 255	     */	      \
		PF_PARITY(p) | /* PF = ((T & 7) ^ Bo).parity */	      \
		nf	       /* NF = IO.7		     */


#define INXR(hl_operator, memptr_operator)			 \
	zuint8 io = IN(BC);					 \
	zuint8 nf = (io >> 6) & NF;				 \
	zuint  t;						 \
	zuint8 hcf, p;						 \
								 \
	WRITE(HL hl_operator, io);				 \
	t = (zuint)io + (zuint8)(MEMPTR = BC memptr_operator 1); \
	hcf = (t > 255) ? HCF : 0;				 \
	p = (t & 7) ^ --B;					 \
	INXR_OTXR_COMMON;					 \
	PC += 2;						 \
	return 16


#define OTXR(hl_operator, memptr_operator) \
	zuint8 io  = READ(HL hl_operator); \
	zuint8 nf  = (io >> 6) & NF;	   \
	zuint  t   = (zuint)io + L;	   \
	zuint8 hcf = (t > 255) ? HCF : 0;  \
	zuint8 p   = (t & 7) ^ --B;	   \
					   \
	OUT(BC, io);			   \
	INXR_OTXR_COMMON;		   \
	MEMPTR = BC memptr_operator 1;	   \
	PC += 2;			   \
	return 16


#define EXIT_HALT      \
	HALT_LINE = 0; \
	if (self->halt != Z_NULL) self->halt(CONTEXT, 0)


/* MARK: - Instructions: 8-Bit Load Group */
/*----------------------------------------------------------------------------.
|			0	1	2	3	  Flags	    T-states  |
|  Assembly		76543210765432107654321076543210  SZYHXPNC     12345  |
|  -------------------	--------------------------------  --------  --------  |
|  ld J,K		01jjjkkk			  ........   4:4      |
|* ld O,P		<--XY-->01oooppp		  ........   8:44     |
|  ld J,BYTE		00jjj110<-BYTE->		  ........   7:43     |
|* ld O,BYTE		<--XY-->00ooo110<-BYTE->	  ........  11:443    |
|  ld J,(hl)		01jjj110			  ........   7:43     |
|  ld J,(XY+OFFSET)	<--XY-->01jjj110<OFFSET>	  ........  19:44353  |
|  ld (hl),K		01110kkk			  ........   7:43     |
|  ld (XY+OFFSET),K	<--XY-->01110kkk<OFFSET>	  ........  19:44353  |
|  ld (hl),BYTE		<--36--><-BYTE->		  ........  10:433    |
|  ld (XY+OFFSET),BYTE	<--XY--><--36--><OFFSET><-BYTE->  ........  19:44353  |
|  ld a,(bc)		<--0A-->			  ........   7:43     |
|  ld a,(de)		<--1A-->			  ........   7:43     |
|  ld a,(WORD)		<--3A--><-----WORD----->	  ........  13:4333   |
|  ld (bc),a		<--02-->			  ........   7:43     |
|  ld (de),a		<--12-->			  ........   7:43     |
|  ld (WORD),a		<--32--><-----WORD----->	  ........  13:4333   |
|  ld a,i		<--ED--><--57-->		  szy0x*0.   9:45     |
|  ld a,r		<--ED--><--5F-->		  szy0x*0.   9:45     |
|  ld i,a		<--ED--><--47-->		  ........   9:45     |
|  ld r,a		<--ED--><--4F-->		  ........   9:45     |
|-----------------------------------------------------------------------------|
| (*) Undocumented instruction.						      |
'============================================================================*/

INSN(ld_J_K	    ) {Q_0 J0 = K0; PC++;					 return	 4;}
INSN(ld_O_P	    ) {Q_0 O  = P;  PC += 2;					 return	 4;}
INSN(ld_J_BYTE	    ) {Q_0 J0 = FETCH((PC += 2) - 1);				 return	 7;}
INSN(ld_O_BYTE	    ) {Q_0 O  = FETCH((PC += 3) - 1);				 return	 7;}
INSN(ld_J_vhl	    ) {Q_0 J0 = READ(HL); PC++;					 return	 7;}
INSN(ld_J_vXYpOFFSET) {Q_0 J1 = READ(FETCH_XY_EA((PC += 3) - 1));		 return 15;}
INSN(ld_vhl_K	    ) {Q_0 PC++; WRITE(HL, K0);					 return	 7;}
INSN(ld_vXYpOFFSET_K) {Q_0 WRITE(FETCH_XY_EA((PC += 3) - 1), K1);		 return 15;}
INSN(ld_vhl_BYTE    ) {Q_0 WRITE(HL, FETCH((PC += 2) - 1));			 return 10;}
INSN(ld_a_vbc	    ) {Q_0 MEMPTR = BC + 1; A = READ(BC); PC++;			 return	 7;}
INSN(ld_a_vde	    ) {Q_0 MEMPTR = DE + 1; A = READ(DE); PC++;			 return	 7;}
INSN(ld_a_vWORD	    ) {Q_0 MEMPTR = FETCH_16((PC += 3) - 2); A = READ(MEMPTR++); return 13;}
INSN(ld_vbc_a	    ) {Q_0 PC++; MEMPTRL = C + 1; WRITE(BC, MEMPTRH = A);	 return	 7;}
INSN(ld_vde_a	    ) {Q_0 PC++; MEMPTRL = E + 1; WRITE(DE, MEMPTRH = A);	 return	 7;}
INSN(ld_a_i	    ) {LD_A_IR(I);							   }
INSN(ld_a_r	    ) {LD_A_IR(R_ALL);							   }
INSN(ld_i_a	    ) {NOTIFY(ld_i_a); Q_0 I = A;      PC += 2;			 return	 9;}
INSN(ld_r_a	    ) {NOTIFY(ld_r_a); Q_0 R = R7 = A; PC += 2;			 return	 9;}


INSN(ld_vXYpOFFSET_BYTE)
	{
	zuint16 ea;

	Q_0
	ea = FETCH_XY_EA((PC += 4) - 2);
	WRITE(ea, FETCH(PC - 1));
	return 15;
	}


INSN(ld_vWORD_a)
	{
	zuint16 ea;

	Q_0
	MEMPTRL = (zuint8)((ea = FETCH_16((PC += 3) - 2)) + 1);
	WRITE(ea, MEMPTRH = A);
	return 13;
	}


/* MARK: - Instructions: 16-Bit Load Group */
/*----------------------------------------------------------------------.
|		 0	 1	 2	 3	   Flags     T-states	|
|  Assembly	 76543210765432107654321076543210  SZYHXPNC	123456	|
|  ------------	 --------------------------------  --------  ---------	|
|  ld SS,WORD	 00ss0001<-----WORD----->	   ........  10:433    [1]
|  ld XY,WORD	 <--XY--><--21--><-----WORD----->  ........  14:4433	|
|  ld hl,(WORD)	 <--2A--><-----WORD----->	   ........  16:43333	|
|  ld SS,(WORD)	 <--ED-->01ss1011<-----WORD----->  ........  20:443333	|
|  ld XY,(WORD)	 <--XY--><--2A--><-----WORD----->  ........  20:443333	|
|  ld (WORD),hl	 <--22--><-----WORD----->	   ........  16:43333	|
|  ld (WORD),SS	 <--ED-->01ss0011<-----WORD----->  ........  20:443333	|
|  ld (WORD),XY	 <--XY--><--22--><-----WORD----->  ........  20:443333	|
|  ld sp,hl	 <--F9-->			   ........   6:6	|
|  ld sp,XY	 <--XY--><--F9-->		   ........  10:46	|
|  push TT	 11tt0101			   ........  11:533	|
|  push XY	 <--XY--><--E5-->		   ........  15:4533	|
|  pop TT	 11tt0001			   ........  10:433	|
|  pop XY	 <--XY--><--E1-->		   ........  14:4433	|
|-----------------------------------------------------------------------|
| 1. All versions of Zilog's "Z80 CPU User Manual" have a typo in the	|
|    M-cycles of the instruction.					|
'======================================================================*/

INSN(ld_SS_WORD ) {Q_0 SS0 = FETCH_16((PC += 3) - 2);	   return 10;}
INSN(ld_XY_WORD ) {Q_0 XY  = FETCH_16((PC += 4) - 2);	   return 10;}
INSN(ld_hl_vWORD) {LD_VWORD_COMMON(3); HL  = READ_16(n);   return 16;}
INSN(ld_SS_vWORD) {LD_VWORD_COMMON(4); SS1 = READ_16(n);   return 20;}
INSN(ld_XY_vWORD) {LD_VWORD_COMMON(4); XY  = READ_16(n);   return 16;}
INSN(ld_vWORD_hl) {LD_VWORD_COMMON(3); WRITE_16F(n, HL );  return 16;}
INSN(ld_vWORD_SS) {LD_VWORD_COMMON(4); WRITE_16F(n, SS1);  return 20;}
INSN(ld_vWORD_XY) {LD_VWORD_COMMON(4); WRITE_16F(n, XY );  return 16;}
INSN(ld_sp_hl	) {Q_0 SP = HL; PC++;			   return  6;}
INSN(ld_sp_XY	) {Q_0 SP = XY; PC += 2;		   return  6;}
INSN(push_TT	) {Q_0 PC++;	PUSH(TT);		   return 11;}
INSN(push_XY	) {Q_0 PC += 2; PUSH(XY);		   return 11;}
INSN(pop_TT	) {Q_0 TT = READ_16(SP); SP += 2; PC++;	   return 10;}
INSN(pop_XY	) {Q_0 XY = READ_16(SP); SP += 2; PC += 2; return 10;}


/* MARK: - Instructions: Exchange, Block Transfer and Search Groups */
/*-------------------------------------------------------------.
|	       0       1	 Flags	   T-states	       |
|  Assembly    7654321076543210	 SZYHXPNC  !0 123456  =0 1234  |
|  ----------  ----------------	 --------  ------------------  |
|  ex de,hl    <--EB-->		 ........   4:4		       |
|  ex af,af'   <--08-->		 ........   4:4		       |
|  exx	       <--D9-->		 ........   4:4		       |
|  ex (sp),hl  <--E3-->		 ........  19:43435	       |
|  ex (sp),XY  <--XY--><--E3-->	 ........  23:443435	       |
|  ldi	       <--ED--><--A0-->	 ..*0**0.  16:4435	       |
|  ldir	       <--ED--><--B0-->	 ..*0*00.  21:44355   16:4435  |
|  ldd	       <--ED--><--A8-->	 ..*0**0.  16:4435	       |
|  lddr	       <--ED--><--B8-->	 ..*0*00.  21:44355   16:4435  |
|  cpi	       <--ED--><--A1-->	 sz*b**1.  16:4435	       |
|  cpir	       <--ED--><--B1-->	 sz*b**1.  21:44355   16:4435  |
|  cpd	       <--ED--><--A9-->	 sz*b**1.  16:4435	       |
|  cpdr	       <--ED--><--B9-->	 sz*b**1.  21:44355   16:4435  |
'=============================================================*/

INSN(ex_de_hl ) {zuint16 t; Q_0 EX(DE, HL ); PC++;			     return 4;}
INSN(ex_af_af_) {zuint16 t; Q_0 EX(AF, AF_); PC++;			     return 4;}
INSN(exx      ) {zuint16 t; Q_0 EX(BC, BC_); EX(DE, DE_); EX(HL, HL_); PC++; return 4;}
INSN(ex_vsp_hl) {EX_VSP(HL, PC++   );						      }
INSN(ex_vsp_XY) {EX_VSP(XY, PC += 2);						      }
INSN(ldi      ) {LDX (++);							      }
INSN(ldir     ) {LDXR(++);							      }
INSN(ldd      ) {LDX (--);							      }
INSN(lddr     ) {LDXR(--);							      }
INSN(cpi      ) {CPX (++);							      }
INSN(cpir     ) {CPXR(++);							      }
INSN(cpd      ) {CPX (--);							      }
INSN(cpdr     ) {CPXR(--);							      }


/* MARK: - Instructions: 8-Bit Arithmetic and Logical Group */
/*-------------------------------------------------------------------.
|		      0	      1	      2		Flags	  T-states   |
|  Assembly	      765432107654321076543210	SZYHXPNC     123456  |
|  -----------------  ------------------------	--------  ---------  |
|  U [a,]K	      10uuukkk			sz||||||   4:4	     |
|* U [a,]P	      <--XY-->10uuuppp		sz||||||   8:44	     |
|  U [a,]BYTE	      11uuu110<-BYTE->		sz||||||   7:43	     |
|  U [a,](hl)	      10uuu110			sz||||||   7:43	     |
|  U [a,](XY+OFFSET)  <--XY-->10uuu110<OFFSET>	sz||||||  19:44353   |
|  V J		      00jjj10v			szy|xv|.   4:4	     |
|* V O		      <--XY-->00ooo10v		szy|xv|.   8:44	     |
|  V (hl)	      0011010v			szy|xv|.  11:443     |
|  V (XY+OFFSET)      <--XY-->0011010v<OFFSET>	szy|xv|.  23:443543  |
|--------------------------------------------------------------------|
| (*) Undocumented instruction.					     |
| (|) The flag is explained in table U/V.			     |
'===================================================================*/

INSN(U_a_K	   ) {U0(K0); PC++;							return	4;}
INSN(U_a_P	   ) {U1(P ); PC += 2;							return	4;}
INSN(U_a_BYTE	   ) {U0(FETCH((PC += 2) - 1));						return	7;}
INSN(U_a_vhl	   ) {U0(READ(HL)); PC++;						return	7;}
INSN(U_a_vXYpOFFSET) {U1(READ(FETCH_XY_EA((PC += 3) - 1)));				return 15;}
INSN(V_J	   ) {zuint8 *j = &J0; *j = V0(*j); PC++;				return	4;}
INSN(V_O	   ) {zuint8 *o = &O;  *o = V1(*o); PC += 2;				return	4;}
INSN(V_vhl	   ) {PC++; WRITE(HL, V0(READ(HL)));					return 11;}
INSN(V_vXYpOFFSET  ) {zuint16 ea = FETCH_XY_EA((PC += 3) - 1); WRITE(ea, V1(READ(ea))); return 19;}


/* MARK: - Instructions: General-Purpose Arithmetic and CPU Control Groups */
/*-------------------------------------------------.
|	     0	     1	       Flags	 T-states  |
|  Assembly  7654321076543210  SZYHXPNC	   12	   |
|  --------  ----------------  --------	 --------  |
|  daa	     <--27-->	       szy^xp.*	 4:4	   |
|  cpl	     <--2F-->	       ..y1x.1.	 4:4	   |
|- neg	     <--ED-->01***100  szybxv1b	 8:44	   |
|  ccf	     <--3F-->	       ..***.0~	 4:4	   |
|  scf	     <--37-->	       ..*0*.01	 4:4	   |
|  nop	     <--00-->	       ........	 4:4	   |
|  halt	     <--76-->	       ........	 4:4	   |
|  di	     <--F3-->	       ........	 4:4	   |
|  ei	     <--FB-->	       ........	 4:4	   |
|- im 0	     <--ED-->01*0*110  ........	 8:44	   |
|- im 1	     <--ED-->01*10110  ........	 8:44	   |
|- im 2	     <--ED-->01*11110  ........	 8:44	   |
|--------------------------------------------------|
| (-) The instruction has undocumented opcodes.	   |
'=================================================*/

INSN(nop ) {Q_0 PC++;		 return 4;}
INSN(im_0) {Q_0 IM = 0; PC += 2; return 8;}
INSN(im_1) {Q_0 IM = 1; PC += 2; return 8;}
INSN(im_2) {Q_0 IM = 2; PC += 2; return 8;}


INSN(daa)
	{
#	ifdef Z80_WITH_PRECOMPUTED_DAA
		zuint16 afi = AF;

#		ifdef Z80_WITH_Q
			Q = (zuint8)
#		endif
		(AF = daa_af_table[
			( afi		   >> 8) |
			((afi & (CF | NF)) << 8) |
			((afi & HF)	   << 6)]);
#	else
		zuint8 cf = A > 0x99, t = ((F & HF) || (A & 0xF) > 9) ? 6 : 0;

		if (F_C || cf) t |= 0x60;
		t = (F & NF) ? A - t : A + t;

		FLAGS = (zuint8)(
			(F & (NF | CF)) | /* NF unchanged; CF dominant */
			(t & SYXF)	| /* SF = sign; YF = Y; XF = X */
			ZF_ZERO(t)	| /* ZF = zero		       */
			((A ^ t) & HF)	| /* HF = Ai.4 != Ao.4	       */
			PF_PARITY(t)	| /* PF = parity	       */
			cf);		  /* CF |= 1 (if BCD carry)    */

		A = t;
#	endif

	PC++;
	return 4;
	}


INSN(cpl)
	{
	FLAGS = F_SZPC		 | /* SF, ZF, PF, CF unchanged */
		((A = ~A) & YXF) | /* YF = Y; XF = X	       */
		HF | NF;	   /* HF, NF = 1	       */

	PC++;
	return 4;
	}


INSN(neg)
	{
	zuint8 t = (zuint8)-A;

	FLAGS = (zuint8)(
		(t & SYXF)	  | /* SF = sign; YF = Y; XF = X */
		ZF_ZERO(t)	  | /* ZF = zero		 */
		((A ^ t) & HF)	  | /* HF = half-borrow		 */
		((t == 128) << 2) | /* PF = overflow		 */
		NF		  | /* NF = 1			 */
		!!A);		    /* CF = borrow (not 0)	 */

	A = t;
	PC += 2;
	return 8;
	}


/*---------------------------------------------------------------------------.
| `ccf` and `scf` are the only instructions in which Q affects the flags.    |
| Patrik Rak cracked the behavior of YF and XF in 2012, confirming that they |
| are taken, respectively, from bits 5 and 3 of the result of `(Q ^ F) | A`  |
| [1, 2]. This applies to all Zilog Z80 models, both NMOS and CMOS. In 2018, |
| David Banks (AKA hoglet) discovered that at least some ST CMOS models do   |
| not set XF according to this formula and instead take this flag from bit 3 |
| of A, whereas NEC NMOS models take both flags from A [3].		     |
|									     |
| References:								     |
| 1. https://worldofspectrum.org/forums/discussion/20345		     |
| 2. https://worldofspectrum.org/forums/discussion/41704		     |
| 3. Banks, David (2018-08-21). "Undocumented Z80 Flags" rev. 1.0.	     |
|     * https://github.com/hoglet67/Z80Decoder/wiki/Undocumented-Flags	     |
|     * https://stardot.org.uk/forums/download/file.php?id=39831	     |
'===========================================================================*/

INSN(ccf)
	{
	FLAGS = (zuint8)(
		(F_SZPC ^ CF) | /* SF, ZF, PF unchanged; CF = ~CFi	    */
		/* Zilog:    YF = A.5 | (YFi ^ YQi); XF = A.3 | (XFi ^ XQi) */
		/* ST CMOS:  YF = A.5 | (YFi ^ YQi); XF = A.3		    */
		/* NEC NMOS: YF = A.5; XF = A.3				    */
#		ifdef Z80_WITH_Q
			((((F ^ Q) & OPTIONS) | A) & YXF) |
#		else
			(A & YXF) |
#		endif
		(F_C << 4)); /* HF = CFi */
			     /* NF = 0	 */
	PC++;
	return 4;
	}


INSN(scf)
	{
	FLAGS = F_SZP | /* SF, ZF, PF unchanged				    */
		/* Zilog:    YF = A.5 | (YFi ^ YQi); XF = A.3 | (XFi ^ XQi) */
		/* ST CMOS:  YF = A.5 | (YFi ^ YQi); XF = A.3		    */
		/* NEC NMOS: YF = A.5; XF = A.3				    */
#		ifdef Z80_WITH_Q
			((((F ^ Q) & OPTIONS) | A) & YXF) |
#		else
			(A & YXF) |
#		endif
		CF; /* CF = 1	  */
		    /* HF, NF = 0 */
	PC++;
	return 4;
	}


/*-----------------------------------------------------------------------------.
| The `halt` instruction enables the HALT state after PC is incremented during |
| the opcode fetch. The CPU neither decrements nor avoids incrementing PC "so  |
| that the instruction is re-executed" as Sean Young writes in section 5.4 of  |
| "The Undocumented Z80 Documented". During the HALT state, the CPU repeatedly |
| executes an internal NOP operation. Each NOP consists of 1 M1 cycle of 4     |
| T-states that fetches (and disregards) the next opcode after `halt` without  |
| incrementing PC. This opcode is read again and again until an exit condition |
| occurs (i.e., INT, NMI or RESET).					       |
|									       |
| This was first documented by Tony Brewer in 2014, and was later verified on  |
| real hardware with the HALT2INT test written by Mark Woodmass (AKA Woody) in |
| 2021.									       |
|									       |
| References:								       |
| * Brewer, Tony (2014-12). "Z80 Special Reset".			       |
|     * http://primrosebank.net/computers/z80/z80_special_reset.htm	       |
| * https://stardot.org.uk/forums/viewtopic.php?p=357136#p357136	       |
'=============================================================================*/

#ifdef Z80_WITH_SPECIAL_RESET
	static Insn const insn_table[256];
#endif

INSN(halt)
	{
	if (!HALT_LINE)
		{
		if (!RESUME)
			{
			Q_0
			PC++;

			if ((self->cycles += 4) >= self->cycle_limit)
				{
				RESUME = Z80_RESUME_HALT;
				return 0;
				}

			if (REQUEST) return 0;
			RESUME = Z80_RESUME_HALT;
			}

		HALT_LINE = 1;

		if (self->halt != Z_NULL)
			{
			self->halt(CONTEXT, 1);
			if (self->cycles >= self->cycle_limit) return 0;
			}
		}

	if (self->nop == Z_NULL || (OPTIONS & Z80_OPTION_HALT_SKIP))
		{
		zusize nop_cycles = self->cycle_limit - self->cycles;

		nop_cycles += (4 - (nop_cycles & 3)) & 3;
		R += (zuint8)(nop_cycles >> 2);
		self->cycles += nop_cycles;
		}

#	ifdef Z80_WITH_SPECIAL_RESET
		else	{
			zuint8 opcode;

			do	{
				R++;
				opcode = self->nop(CONTEXT, PC);
				self->cycles += 4;

				if (REQUEST)
					{
					RESUME = 0;

					if (REQUEST & Z80_REQUEST_SPECIAL_RESET)
						{
						HALT_LINE = 0;

						if (self->halt != Z_NULL)
							self->halt(CONTEXT, Z80_HALT_EXIT_EARLY);

						if ((DATA[0] = opcode) != 0x76)
							{
							self->cycles -= 4;
							PC--;
							return insn_table[opcode](self);
							}
						}

					return 0;
					}
				}
			while (self->cycles < self->cycle_limit);

			DATA[2] = opcode;
			}

#	else
		else do	{
			R++;
			(void)self->nop(CONTEXT, PC);
			self->cycles += 4;

			if (REQUEST)
				{
				RESUME = 0;
				return 0;
				}
			}
		while (self->cycles < self->cycle_limit);
#	endif

	return 0;
	}


INSN(di)
	{
	Q_0
	IFF1 = IFF2 = 0;
	REQUEST &= ~(zuint8)Z80_REQUEST_INT;
	PC++;
	return 4;
	}


INSN(ei)
	{
	Q_0
	IFF1 = IFF2 = 1;
	if (INT_LINE) REQUEST |= Z80_REQUEST_INT;
	PC++;
	return 4;
	}


/* MARK: - Instructions: 16-Bit Arithmetic Group */
/*--------------------------------------------------.
|	      0	      1		Flags	  T-states  |
|  Assembly   7654321076543210	SZYHXPNC     1234   |
|  ---------  ----------------	--------  --------  |
|  add hl,SS  00ss1001		..YCX.0c  11:443    |
|  adc hl,SS  <--ED-->01ss1010	szYCXv0c  15:4443   |
|  sbc hl,SS  <--ED-->01ss0010	szYBXv1b  15:4443   |
|  add XY,WW  <--XY-->00ww1001	..YCX.0c  15:4443   |
|  inc SS     00ss0011		........   6:6	    |
|  inc XY     <--XY--><--23-->	........  10:46	    |
|  dec SS     00ss1011		........   6:6	    |
|  dec XY     <--XY--><--2B-->	........  10:46	    |
'==================================================*/

INSN(add_hl_SS) {ADD_16(HL, SS0, PC++);		}
INSN(adc_hl_SS) {ADC_SBC_HL_SS(+, ~ss, Z_EMPTY);}
INSN(sbc_hl_SS) {ADC_SBC_HL_SS(-,  ss, | NF   );}
INSN(add_XY_WW) {ADD_16(XY, WW, PC += 2);	}
INSN(inc_SS   ) {Q_0 (SS0)++; PC++;    return 6;}
INSN(inc_XY   ) {Q_0 XY++;    PC += 2; return 6;}
INSN(dec_SS   ) {Q_0 (SS0)--; PC++;    return 6;}
INSN(dec_XY   ) {Q_0 XY--;    PC += 2; return 6;}


/* MARK: - Instructions: Rotate and Shift Group */
/*-------------------------------------------------------------------------.
|		    0	    1	    2	    3	      Flags	T-states   |
|  Assembly	    76543210765432107654321076543210  SZYHXPNC	   123456  |
|  ---------------  --------------------------------  --------	---------  |
|  rlca		    <--07-->			      ..y0x.0=	 4:4	   |
|  rla		    <--17-->			      ..y0x.0=	 4:4	   |
|  rrca		    <--0F-->			      ..y0x.0=	 4:4	   |
|  rra		    <--1F-->			      ..y0x.0=	 4:4	   |
|- G K		    <--CB-->00gggkkk		      szy0xp0=	 8:44	   |
|- G (hl)	    <--CB-->00ggg110		      szy0xp0=	15:4443	   |
|- G (XY+OFFSET)    <--XY--><--CB--><OFFSET>00ggg110  szy0xp0=	23:443543  |
|* G (XY+OFFSET),K  <--XY--><--CB--><OFFSET>00gggkkk  szy0xp0=	23:443543  |
|  rld		    <--ED--><--6F-->		      szy0xp0.	18:44343   |
|  rrd		    <--ED--><--67-->		      szy0xp0.	18:44343   |
|--------------------------------------------------------------------------|
| (-) The instruction has undocumented [pseudo-]opcodes.		   |
| (*) Undocumented instruction.						   |
'=========================================================================*/

INSN(rlca	   ) {A = ROL(A); FLAGS = F_SZP | (A & YXCF); PC++;	 return	 4;}
INSN(rla	   ) {RXA(A >> 7, <<, F_C);					   }
INSN(rrca	   ) {A = ROR(A); FLAGS = F_SZP | A_YX | (A >> 7); PC++; return	 4;}
INSN(rra	   ) {RXA(A & 1, >>, (F << 7));					   }
INSN(G_K	   ) {zuint8 *k = &K1; *k = G1(*k);			 return	 8;}
INSN(G_vhl	   ) {WRITE(HL, G1(READ(HL)));				 return 15;}
INSN(G_vXYpOFFSET  ) {zuint16 ea = MEMPTR; WRITE(ea,	  G3(READ(ea))); return 19;}
INSN(G_vXYpOFFSET_K) {zuint16 ea = MEMPTR; WRITE(ea, K3 = G3(READ(ea))); return 19;}
INSN(rld	   ) {RXD(<< 4, & 0xF, >> 4);					   }
INSN(rrd	   ) {RXD(>> 4, << 4, & 0xF);					   }


/* MARK: - Instructions: Bit Set, Reset and Test Group */
/*---------------------------------------------------------------------------.
|		      0	      1	      2	      3		Flags	  T-states   |
|  Assembly	      76543210765432107654321076543210	SZYHXPNC     123456  |
|  -----------------  --------------------------------	--------  ---------  |
|  bit N,K	      <--CB-->01nnnkkk			sz*1*z0.   8:44	     |
|  bit N,(hl)	      <--CB-->01nnn110			sz*1*z0.  12:444    [1]
|- bit N,(XY+OFFSET)  <--XY--><--CB--><OFFSET>01nnn***	sz*1*z0.  20:44354   |
|  M N,K	      <--CB-->1mnnnkkk			........   8:44	     |
|  M N,(hl)	      <--CB-->1mnnn110			........  15:4443    |
|  M N,(XY+OFFSET)    <--XY--><--CB--><OFFSET>1mnnn110	........  23:443543  |
|* M N,(XY+OFFSET),K  <--XY--><--CB--><OFFSET>1mnnnkkk	........  23:443543  |
|----------------------------------------------------------------------------|
| (-) The instruction has undocumented pseudo-opcodes.			     |
| (*) Undocumented instruction.						     |
|----------------------------------------------------------------------------|
| 1. All versions of Zilog's "Z80 CPU User Manual" have a typo in the	     |
|    T-states of the instruction.					     |
'===========================================================================*/

INSN(M_N_K	     ) {zuint8 *k = &K1; *k = M1(*k);			   return  8;}
INSN(M_N_vhl	     ) {WRITE(HL, M1(READ(HL)));			   return 15;}
INSN(M_N_vXYpOFFSET  ) {zuint16 ea = MEMPTR; WRITE(ea,	    M3(READ(ea))); return 19;}
INSN(M_N_vXYpOFFSET_K) {zuint16 ea = MEMPTR; WRITE(ea, K3 = M3(READ(ea))); return 19;}


INSN(bit_N_K)
	{
	zuint8 k = K1;
	zuint8 t = k & (1U << N(1));

	/*----------------------------------------------------------.
	| In section 4.1 of "The Undocumented Z80 Documented" (all  |
	| versions), Sean Young says that YF and XF are taken from  |
	| the value resulting from the bit test operation, but this |
	| seems not to be true. They are copies of bits 5 and 3 of  |
	| the register containing the value to be tested (K).	    |
	'==========================================================*/
	FLAGS = (t ? t & SF : ZPF) | /* SF = sign; ZF, PF = zero   */
		(k & YXF)	   | /* YF = K.5; XF = K.3	   */
		HF		   | /* HF = 1			   */
		F_C;		     /* CF unchanged		   */
				     /* NF = 0			   */
	return 8;
	}


INSN(bit_N_vhl)
	{
	zuint8 t = READ(HL) & (1U << N(1));

	/*----------------------------------------------------------------.
	| This is the only instruction in which MEMPTR affects the flags. |
	| YF and XF are taken, respectively, from bits 13 and 11 of this  |
	| internal register whose behavior was cracked in 2006 by boo_boo |
	| and Vladimir Kladov. Official schematics refer to this register |
	| as WZ, but this emulator uses the name "MEMPTR" to honor those  |
	| who cracked it.						  |
	|								  |
	| References:							  |
	| * https://zxpress.ru/zxnet/zxnet.pc/5909			  |
	| * boo_boo; Kladov, Vladimir (2006-03-29). "MEMPTR, Esoteric	  |
	|   Register of the Zilog Z80 CPU".				  |
	|     * https://zx-pk.ru/showpost.php?p=43688			  |
	|     * https://zx-pk.ru/attachment.php?attachmentid=2984	  |
	|     * https://zx-pk.ru/showpost.php?p=43800			  |
	|     * https://zx-pk.ru/attachment.php?attachmentid=2989	  |
	'================================================================*/
	FLAGS = (t ? t & SF : ZPF) | /* SF = sign; ZF, PF = zero	 */
		(MEMPTRH & YXF)	   | /* YF = MEMPTRH.5; XF = MEMPTRH.3	 */
		HF		   | /* HF = 1				 */
		F_C;		     /* CF unchanged			 */
				     /* NF = 0				 */
	return 12;
	}


INSN(bit_N_vXYpOFFSET)
	{
	zuint8 t = READ(MEMPTR) & (1U << N(3));

	FLAGS = (t ? t & SF : ZPF) | /* SF sign; ZF, PF = zero */
		(MEMPTRH & YXF)	   | /* YF = EA.13; XF = EA.11 */
		HF		   | /* HF = 1		       */
		F_C;		     /* CF unchanged	       */
				     /* NF = 0		       */
	return 16;
	}


/* MARK: - Instructions: Jump Group */
/*----------------------------------------------------------------.
|		0	1	2	  Flags	    T-states	  |
|  Assembly	765432107654321076543210  SZYHXPNC   Y 123  N 12  |
|  -----------	------------------------  --------  ------------  |
|  jp WORD	<--C3--><-----WORD----->  ........  10:433	  |
|  jp Z,WORD	11zzz010<-----WORD----->  ........  10:433	  |
|  jr OFFSET	<--18--><OFFSET>	  ........  12:435	  |
|  jr Z,OFFSET	001zz000<OFFSET>	  ........  12:435  7:43  |
|  jp (hl)	<--E9-->		  ........   4:4	  |
|  jp (XY)	<--XY--><--E9-->	  ........   8:44	  |
|  djnz OFFSET	<--10--><OFFSET>	  ........  13:535  8:53  |
'================================================================*/

INSN(jp_WORD	) {Q_0 MEMPTR = PC = FETCH_16(PC + 1);			       return 10;}
INSN(jp_Z_WORD	) {Q_0 MEMPTR = FETCH_16(PC + 1); PC = Z(7) ? MEMPTR : PC + 3; return 10;}
INSN(jr_OFFSET	) {Q_0 MEMPTR = (PC += 2 + (zsint8)FETCH(PC + 1));	       return 12;}
INSN(jr_Z_OFFSET) {DJNZ_JR_Z(Z(3), 12, 7);						 }
INSN(jp_hl	) {Q_0 PC = HL;						       return  4;}
INSN(jp_XY	) {Q_0 PC = XY;						       return  4;}
INSN(djnz_OFFSET) {DJNZ_JR_Z(--B, 13, 8);						 }


/* MARK: - Instructions: Call and Return Group */
/*--------------------------------------------------------------------.
|		0	1	2	  Flags	    T-states	      |
|  Assembly	765432107654321076543210  SZYHXPNC   Y 123     N 123  |
|  -----------	------------------------  --------  ----------------  |
|  call WORD	<--CD--><-----WORD----->  ........  17:43433	      |
|  call Z,WORD	11zzz100<-----WORD----->  ........  17:43433  10:433  |
|  ret		<--C9-->		  ........  10:433	      |
|  ret Z	11zzz000		  ........  11:533     5:5    |
|- reti/retn	<--ED-->01***101	  ........  14:4433	      |
|  rst N	11nnn111		  ........  11:533	      |
|---------------------------------------------------------------------|
| (-) The instruction has undocumented opcodes. The `reti` mnemonic   |
|     is used to represent the ED4Dh opcode, which is recognized by   |
|     the Z80 CTC chip. All other opcodes are represented as `retn`.  |
'====================================================================*/

INSN(call_WORD) {zuint16 pci = PC; Q_0 MEMPTR = PC = FETCH_16(pci + 1); PUSH(pci + 3); return 17;}
INSN(ret      ) {Q_0 RET;							       return 10;}
INSN(ret_Z    ) {Q_0 if (Z(7)) {RET; return 11;} PC++;				       return  5;}
INSN(reti     ) {RETX(reti);									 }
INSN(retn     ) {RETX(retn);									 }
INSN(rst_N    ) {zuint16 pci = PC; Q_0 MEMPTR = PC = DATA[0] & 56; PUSH(pci + 1);      return 11;}


INSN(call_Z_WORD)
	{
	zuint16 pci;

	Q_0
	MEMPTR = FETCH_16((pci = PC) + 1); /* Always read */

	if (Z(7))
		{
		PC = MEMPTR;
		PUSH(pci + 3);
		return 17;
		}

	PC += 3;
	return 10;
	}


/* MARK: - Instructions: Input and Output Group */
/*--------------------------------------------------------------.
|		 0	 1	   Flags     T-states		|
|  Assembly	 7654321076543210  SZYHXPNC  !0 12345  =0 1234	|
|  ------------	 ----------------  --------  -----------------	|
|  in a,(BYTE)	 <--DB--><-BYTE->  ........  11:434		|
|  in J,(c)	 <--ED-->01jjj000  szy0xp0.  12:444		|
|* in (c)	 <--ED--><--70-->  szy0xp0.  12:444		|
|  ini		 <--ED--><--A2-->  ********  16:4543	       [1]
|  inir		 <--ED--><--B2-->  ********  21:45435  16:4543 [1]
|  ind		 <--ED--><--AA-->  ********  16:4543	       [1]
|  indr		 <--ED--><--BA-->  ********  21:45435  16:4543 [1]
|  out (BYTE),a	 <--D3--><-BYTE->  ........  11:434		|
|  out (c),J	 <--ED-->01jjj001  ........  12:444		|
|* out (c),0	 <--ED--><--71-->  ........  12:444		|
|  outi		 <--ED--><--A3-->  ********  16:4534		|
|  otir		 <--ED--><--B3-->  ********  21:45345  16:4534	|
|  outd		 <--ED--><--AB-->  ********  16:4534		|
|  otdr		 <--ED--><--BB-->  ********  21:45345  16:4534	|
|---------------------------------------------------------------|
| (*) Undocumented instruction.					|
|---------------------------------------------------------------|
| 1. All versions of Zilog's "Z80 CPU User Manual" have typos	|
|    in the T-states of the instruction.			|
'==============================================================*/

INSN(in_J_vc ) {IN_VC(J1 = t;);					     }
INSN(in_vc   ) {IN_VC(Z_EMPTY);					     }
INSN(ini     ) {INX (++, +);					     }
INSN(inir    ) {INXR(++, +);					     }
INSN(ind     ) {INX (--, -);					     }
INSN(indr    ) {INXR(--, -);					     }
INSN(out_vc_J) {Q_0 PC += 2; MEMPTR = BC + 1; OUT(BC, J1); return 12;}
INSN(outi    ) {OUTX(++, +);					     }
INSN(otir    ) {OTXR(++, +);					     }
INSN(outd    ) {OUTX(--, -);					     }
INSN(otdr    ) {OTXR(--, -);					     }


INSN(in_a_vBYTE)
	{
	zuint16 t;

	Q_0

	/*--------------------------------------------------------------------.
	| In "MEMPTR, Esoteric Register of the Zilog Z80 CPU", boo_boo says   |
	| that MEMPTR is set to `((A << 8) | BYTE) + 1`. This causes a carry  |
	| from the LSbyte of the port number to MEMPTRH if BYTE is 255, which |
	| differs from all other instructions where MEMPTRH is set to A, but  |
	| it has been verified on real hardware with the IN-MEMPTR test.      |
	'====================================================================*/
	MEMPTR = (t = (zuint16)(((zuint16)A << 8) | FETCH((PC += 2) - 1))) + 1;

	A = IN(t);
	return 11;
	}


INSN(out_vBYTE_a)
	{
	zuint8 t;

	Q_0
	MEMPTRL = (t = FETCH((PC += 2) - 1)) + 1;
	MEMPTRH = A;
	OUT((zuint16)(((zuint16)A << 8) | t), A);
	return 11;
	}


/*-----------------------------------------------------------------------------.
| The `out (c),0` instruction behaves as `out (c),255` on the Zilog Z80 CMOS.  |
| This was first discovered by Simon Cooke, who reported it on Usenet in 1996  |
| [1, 2]. Later, in 2004, Colin Piggot rediscovered it with his SAM Coupé when |
| running a demo for SCPDU 6, coincidentally written by Simon Cooke [1]. In    |
| 2008, this was once again rediscovered by the MSX community [1, 3].	       |
|									       |
| References:								       |
| 1. https://sinclair.wiki.zxnet.co.uk/wiki/Z80				       |
| 2. https://groups.google.com/g/comp.os.cpm/c/HfSTFpaIkuU/m/KotvMWu3bZoJ      |
| 3. https://msx.org/forum/development/msx-development/bug-z80-emulation-or-tr |
|    -hw								       |
'=============================================================================*/

INSN(out_vc_0)
	{
	Q_0
	PC += 2;
	MEMPTR = BC + 1;
	OUT(BC, (zuint8)0 - (OPTIONS & (zuint8)Z80_OPTION_OUT_VC_255));
	return 12;
	}


/* MARK: - Instructions: Optimizations */

INSN(nop_nop) {Q_0; PC += 2; return 4;}


/* MARK: - Instruction Function Tables */

INSN(cb_prefix	 );
INSN(ed_prefix	 );
INSN(dd_prefix	 );
INSN(fd_prefix	 );
INSN(xy_cb_prefix);
INSN(xy_xy	 );
INSN(ed_illegal	 );
INSN(xy_illegal	 );
INSN(hook	 );

#ifdef Z80_WITH_UNOFFICIAL_RETI
#	define reti_retn reti
#else
#	define reti_retn retn
#endif

static Insn const insn_table[256] = {
/*	0	     1		 2	      3		   4		5	  6	       7	 8	      9		 A	      B		  C	       D	  E	     F */
/* 0 */ nop,	     ld_SS_WORD, ld_vbc_a,    inc_SS,	   V_J,		V_J,	  ld_J_BYTE,   rlca,	 ex_af_af_,   add_hl_SS, ld_a_vbc,    dec_SS,	  V_J,	       V_J,	  ld_J_BYTE, rrca,
/* 1 */ djnz_OFFSET, ld_SS_WORD, ld_vde_a,    inc_SS,	   V_J,		V_J,	  ld_J_BYTE,   rla,	 jr_OFFSET,   add_hl_SS, ld_a_vde,    dec_SS,	  V_J,	       V_J,	  ld_J_BYTE, rra,
/* 2 */ jr_Z_OFFSET, ld_SS_WORD, ld_vWORD_hl, inc_SS,	   V_J,		V_J,	  ld_J_BYTE,   daa,	 jr_Z_OFFSET, add_hl_SS, ld_hl_vWORD, dec_SS,	  V_J,	       V_J,	  ld_J_BYTE, cpl,
/* 3 */ jr_Z_OFFSET, ld_SS_WORD, ld_vWORD_a,  inc_SS,	   V_vhl,	V_vhl,	  ld_vhl_BYTE, scf,	 jr_Z_OFFSET, add_hl_SS, ld_a_vWORD,  dec_SS,	  V_J,	       V_J,	  ld_J_BYTE, ccf,
/* 4 */ nop,	     ld_J_K,	 ld_J_K,      ld_J_K,	   ld_J_K,	ld_J_K,	  ld_J_vhl,    ld_J_K,	 ld_J_K,      nop,	 ld_J_K,      ld_J_K,	  ld_J_K,      ld_J_K,	  ld_J_vhl,  ld_J_K,
/* 5 */ ld_J_K,	     ld_J_K,	 nop,	      ld_J_K,	   ld_J_K,	ld_J_K,	  ld_J_vhl,    ld_J_K,	 ld_J_K,      ld_J_K,	 ld_J_K,      nop,	  ld_J_K,      ld_J_K,	  ld_J_vhl,  ld_J_K,
/* 6 */ ld_J_K,	     ld_J_K,	 ld_J_K,      ld_J_K,	   hook,	ld_J_K,	  ld_J_vhl,    ld_J_K,	 ld_J_K,      ld_J_K,	 ld_J_K,      ld_J_K,	  ld_J_K,      nop,	  ld_J_vhl,  ld_J_K,
/* 7 */ ld_vhl_K,    ld_vhl_K,	 ld_vhl_K,    ld_vhl_K,	   ld_vhl_K,	ld_vhl_K, halt,	       ld_vhl_K, ld_J_K,      ld_J_K,	 ld_J_K,      ld_J_K,	  ld_J_K,      ld_J_K,	  ld_J_vhl,  nop,
/* 8 */ U_a_K,	     U_a_K,	 U_a_K,	      U_a_K,	   U_a_K,	U_a_K,	  U_a_vhl,     U_a_K,	 U_a_K,	      U_a_K,	 U_a_K,	      U_a_K,	  U_a_K,       U_a_K,	  U_a_vhl,   U_a_K,
/* 9 */ U_a_K,	     U_a_K,	 U_a_K,	      U_a_K,	   U_a_K,	U_a_K,	  U_a_vhl,     U_a_K,	 U_a_K,	      U_a_K,	 U_a_K,	      U_a_K,	  U_a_K,       U_a_K,	  U_a_vhl,   U_a_K,
/* A */ U_a_K,	     U_a_K,	 U_a_K,	      U_a_K,	   U_a_K,	U_a_K,	  U_a_vhl,     U_a_K,	 U_a_K,	      U_a_K,	 U_a_K,	      U_a_K,	  U_a_K,       U_a_K,	  U_a_vhl,   U_a_K,
/* B */ U_a_K,	     U_a_K,	 U_a_K,	      U_a_K,	   U_a_K,	U_a_K,	  U_a_vhl,     U_a_K,	 U_a_K,	      U_a_K,	 U_a_K,	      U_a_K,	  U_a_K,       U_a_K,	  U_a_vhl,   U_a_K,
/* C */ ret_Z,	     pop_TT,	 jp_Z_WORD,   jp_WORD,	   call_Z_WORD, push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      ret,	 jp_Z_WORD,   cb_prefix,  call_Z_WORD, call_WORD, U_a_BYTE,  rst_N,
/* D */ ret_Z,	     pop_TT,	 jp_Z_WORD,   out_vBYTE_a, call_Z_WORD, push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      exx,	 jp_Z_WORD,   in_a_vBYTE, call_Z_WORD, dd_prefix, U_a_BYTE,  rst_N,
/* E */ ret_Z,	     pop_TT,	 jp_Z_WORD,   ex_vsp_hl,   call_Z_WORD, push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      jp_hl,	 jp_Z_WORD,   ex_de_hl,	  call_Z_WORD, ed_prefix, U_a_BYTE,  rst_N,
/* F */ ret_Z,	     pop_TT,	 jp_Z_WORD,   di,	   call_Z_WORD, push_TT,  U_a_BYTE,    rst_N,	 ret_Z,	      ld_sp_hl,	 jp_Z_WORD,   ei,	  call_Z_WORD, fd_prefix, U_a_BYTE,  rst_N};

static Insn const cb_insn_table[256] = {
/*	0	 1	  2	   3	    4	     5	      6		 7	  8	   9	    A	     B	      C	       D	E	   F */
/* 0 */ G_K,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_vhl,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_K,     G_K,	G_vhl,	   G_K,
/* 1 */ G_K,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_vhl,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_K,     G_K,	G_vhl,	   G_K,
/* 2 */ G_K,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_vhl,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_K,     G_K,	G_vhl,	   G_K,
/* 3 */ G_K,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_vhl,	 G_K,	  G_K,	   G_K,	    G_K,     G_K,     G_K,     G_K,	G_vhl,	   G_K,
/* 4 */ bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K,
/* 5 */ bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K,
/* 6 */ bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K,
/* 7 */ bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_K, bit_N_vhl, bit_N_K,
/* 8 */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* 9 */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* A */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* B */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* C */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* D */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* E */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K,
/* F */ M_N_K,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_vhl,	 M_N_K,	  M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,   M_N_K,	M_N_vhl,   M_N_K};

static Insn const ed_insn_table[256] = {
/*	0	    1		2	    3		 4	     5		 6	     7		 8	     9		 A	     B		  C	      D		  E	      F */
/* 0 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* 1 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* 2 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* 3 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* 4 */ in_J_vc,    out_vc_J,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_0,	     ld_i_a,	 in_J_vc,    out_vc_J,	 adc_hl_SS,  ld_SS_vWORD, neg,	      reti,	  im_0,	      ld_r_a,
/* 5 */ in_J_vc,    out_vc_J,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_1,	     ld_a_i,	 in_J_vc,    out_vc_J,	 adc_hl_SS,  ld_SS_vWORD, neg,	      reti_retn,  im_2,	      ld_a_r,
/* 6 */ in_J_vc,    out_vc_J,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_0,	     rrd,	 in_J_vc,    out_vc_J,	 adc_hl_SS,  ld_SS_vWORD, neg,	      reti_retn,  im_0,	      rld,
/* 7 */ in_vc,	    out_vc_0,	sbc_hl_SS,  ld_vWORD_SS, neg,	     retn,	 im_1,	     ed_illegal, in_J_vc,    out_vc_J,	 adc_hl_SS,  ld_SS_vWORD, neg,	      reti_retn,  im_2,	      ed_illegal,
/* 8 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* 9 */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* A */ ldi,	    cpi,	ini,	    outi,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ldd,	     cpd,	 ind,	     outd,	  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* B */ ldir,	    cpir,	inir,	    otir,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, lddr,	     cpdr,	 indr,	     otdr,	  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* C */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* D */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* E */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal,
/* F */ ed_illegal, ed_illegal, ed_illegal, ed_illegal,	 ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal, ed_illegal,  ed_illegal, ed_illegal, ed_illegal, ed_illegal};

static Insn const xy_insn_table[256] = {
/*	0		 1		  2		   3		    4		     5		      6			  7		   8	       9	   A		B	      C		  D	      E		       F */
/* 0 */ nop_nop,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    V_O,	     V_O,	      ld_O_BYTE,	  xy_illegal,	   xy_illegal, add_XY_WW,  xy_illegal,	xy_illegal,   V_O,	  V_O,	      ld_O_BYTE,       xy_illegal,
/* 1 */ xy_illegal,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    V_O,	     V_O,	      ld_O_BYTE,	  xy_illegal,	   xy_illegal, add_XY_WW,  xy_illegal,	xy_illegal,   V_O,	  V_O,	      ld_O_BYTE,       xy_illegal,
/* 2 */ xy_illegal,	 ld_XY_WORD,	  ld_vWORD_XY,	   inc_XY,	    V_O,	     V_O,	      ld_O_BYTE,	  xy_illegal,	   xy_illegal, add_XY_WW,  ld_XY_vWORD, dec_XY,	      V_O,	  V_O,	      ld_O_BYTE,       xy_illegal,
/* 3 */ xy_illegal,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    V_vXYpOFFSET,    V_vXYpOFFSET,    ld_vXYpOFFSET_BYTE, xy_illegal,	   xy_illegal, add_XY_WW,  xy_illegal,	xy_illegal,   V_O,	  V_O,	      ld_O_BYTE,       xy_illegal,
/* 4 */ nop_nop,	 ld_O_P,	  ld_O_P,	   ld_O_P,	    ld_O_P,	     ld_O_P,	      ld_J_vXYpOFFSET,	  ld_O_P,	   ld_O_P,     nop_nop,	   ld_O_P,	ld_O_P,	      ld_O_P,	  ld_O_P,     ld_J_vXYpOFFSET, ld_O_P,
/* 5 */ ld_O_P,		 ld_O_P,	  nop_nop,	   ld_O_P,	    ld_O_P,	     ld_O_P,	      ld_J_vXYpOFFSET,	  ld_O_P,	   ld_O_P,     ld_O_P,	   ld_O_P,	nop_nop,      ld_O_P,	  ld_O_P,     ld_J_vXYpOFFSET, ld_O_P,
/* 6 */ ld_O_P,		 ld_O_P,	  ld_O_P,	   ld_O_P,	    nop_nop,	     ld_O_P,	      ld_J_vXYpOFFSET,	  ld_O_P,	   ld_O_P,     ld_O_P,	   ld_O_P,	ld_O_P,	      ld_O_P,	  nop_nop,    ld_J_vXYpOFFSET, ld_O_P,
/* 7 */ ld_vXYpOFFSET_K, ld_vXYpOFFSET_K, ld_vXYpOFFSET_K, ld_vXYpOFFSET_K, ld_vXYpOFFSET_K, ld_vXYpOFFSET_K, xy_illegal,	  ld_vXYpOFFSET_K, ld_O_P,     ld_O_P,	   ld_O_P,	ld_O_P,	      ld_O_P,	  ld_O_P,     ld_J_vXYpOFFSET, nop_nop,
/* 8 */ U_a_P,		 U_a_P,		  U_a_P,	   U_a_P,	    U_a_P,	     U_a_P,	      U_a_vXYpOFFSET,	  U_a_P,	   U_a_P,      U_a_P,	   U_a_P,	U_a_P,	      U_a_P,	  U_a_P,      U_a_vXYpOFFSET,  U_a_P,
/* 9 */ U_a_P,		 U_a_P,		  U_a_P,	   U_a_P,	    U_a_P,	     U_a_P,	      U_a_vXYpOFFSET,	  U_a_P,	   U_a_P,      U_a_P,	   U_a_P,	U_a_P,	      U_a_P,	  U_a_P,      U_a_vXYpOFFSET,  U_a_P,
/* A */ U_a_P,		 U_a_P,		  U_a_P,	   U_a_P,	    U_a_P,	     U_a_P,	      U_a_vXYpOFFSET,	  U_a_P,	   U_a_P,      U_a_P,	   U_a_P,	U_a_P,	      U_a_P,	  U_a_P,      U_a_vXYpOFFSET,  U_a_P,
/* B */ U_a_P,		 U_a_P,		  U_a_P,	   U_a_P,	    U_a_P,	     U_a_P,	      U_a_vXYpOFFSET,	  U_a_P,	   U_a_P,      U_a_P,	   U_a_P,	U_a_P,	      U_a_P,	  U_a_P,      U_a_vXYpOFFSET,  U_a_P,
/* C */ xy_illegal,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    xy_illegal,	     xy_illegal,      xy_illegal,	  xy_illegal,	   xy_illegal, xy_illegal, xy_illegal,	xy_cb_prefix, xy_illegal, xy_illegal, xy_illegal,      xy_illegal,
/* D */ xy_illegal,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    xy_illegal,	     xy_illegal,      xy_illegal,	  xy_illegal,	   xy_illegal, xy_illegal, xy_illegal,	xy_illegal,   xy_illegal, xy_xy,      xy_illegal,      xy_illegal,
/* E */ xy_illegal,	 pop_XY,	  xy_illegal,	   ex_vsp_XY,	    xy_illegal,	     push_XY,	      xy_illegal,	  xy_illegal,	   xy_illegal, jp_XY,	   xy_illegal,	xy_illegal,   xy_illegal, xy_illegal, xy_illegal,      xy_illegal,
/* F */ xy_illegal,	 xy_illegal,	  xy_illegal,	   xy_illegal,	    xy_illegal,	     xy_illegal,      xy_illegal,	  xy_illegal,	   xy_illegal, ld_sp_XY,   xy_illegal,	xy_illegal,   xy_illegal, xy_xy,      xy_illegal,      xy_illegal};

static Insn const xy_cb_insn_table[256] = {
/*	0		  1		    2		      3			4		  5		    6		      7			8		  9		    A		      B			C		  D		    E		      F */
/* 0 */ G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,
/* 1 */ G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,
/* 2 */ G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,
/* 3 */ G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET_K,   G_vXYpOFFSET_K,	G_vXYpOFFSET_K,	  G_vXYpOFFSET_K,   G_vXYpOFFSET,     G_vXYpOFFSET_K,
/* 4 */ bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET,
/* 5 */ bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET,
/* 6 */ bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET,
/* 7 */ bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET, bit_N_vXYpOFFSET,
/* 8 */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* 9 */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* A */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* B */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* C */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* D */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* E */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K,
/* F */ M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET_K, M_N_vXYpOFFSET,   M_N_vXYpOFFSET_K};


/* MARK: - Instructions: Prefix Handling */

INSN(cb_prefix)
	{
	R++;
	return cb_insn_table[DATA[1] = FETCH_OPCODE((PC += 2) - 1)](self);
	}


INSN(ed_prefix)
	{
	R++;
	return ed_insn_table[DATA[1] = FETCH_OPCODE(PC + 1)](self);
	}


#define XY_PREFIX(index_register)				      \
	zuint8 cycles;						      \
								      \
	if ((self->cycles += 4) >= self->cycle_limit)		      \
		{						      \
		RESUME = Z80_RESUME_XY;				      \
		return 0;					      \
		}						      \
								      \
	R++;							      \
	XY = index_register;					      \
	cycles = xy_insn_table[DATA[1] = FETCH_OPCODE(PC + 1)](self); \
	index_register = XY;					      \
	return cycles;


INSN(dd_prefix) {XY_PREFIX(IX)}
INSN(fd_prefix) {XY_PREFIX(IY)}


/*-----------------------------------------------------------------------.
| Instructions with the two-byte prefix DDCBh or FDCBh increment R by 2, |
| as only the prefix is fetched by opcode fetch operations (M1 cycles).	 |
| The remaining two bytes are fetched by normal memory read operations.	 |
'=======================================================================*/

INSN(xy_cb_prefix)
	{
	FETCH_XY_EA((PC += 4) - 2);
	return xy_cb_insn_table[DATA[3] = FETCH(PC - 1)](self);
	}


/*-----------------------------------------------------------------------------.
| In a sequence of DDh and/or FDh prefixes, it is the last one that counts, as |
| each prefix overrides the previous one. No matter how long the sequence is,  |
| interrupts can only be responded to after executing the final instruction    |
| once all the prefixes have been fetched. Each prefix takes 4 T-states.       |
'=============================================================================*/

INSN(xy_xy)
	{
	zuint8 cycles;
	zuint8 first_prefix = DATA[0];

	do	{
		PC++;
		DATA[0] = DATA[1];

		if ((self->cycles += 4) >= self->cycle_limit)
			{
			RESUME = Z80_RESUME_XY;
			return 0;
			}

		R++;
		}
	while (IS_XY_PREFIX(DATA[1] = FETCH_OPCODE(PC + 1)));

	if (DATA[0] == first_prefix) return xy_insn_table[DATA[1]](self);

	if (first_prefix == 0xFD)
		{
		XY = IX;
		cycles = xy_insn_table[DATA[1]](self);
		IX = XY;
		XY = IY;
		}

	else	{
		XY = IY;
		cycles = xy_insn_table[DATA[1]](self);
		IY = XY;
		XY = IX;
		}

	return cycles;
	}


/* MARK: - Instructions: Illegal */

/*------------------------------------------------------------------------.
| Illegal opcodes prefixed with EDh are ignored by the CPU. Functionally, |
| these instructions are equivalent to two consecutive `nop` instructions |
| and take a total of 8 T-states.					  |
'========================================================================*/

INSN(ed_illegal)
	{
	if (self->illegal != Z_NULL)
		{
		DATA[2] = 0;
		return self->illegal(self, DATA[1]);
		}

	Q_0
	PC += 2;
	return 8;
	}


/*-------------------------------------------------------------------------.
| Illegal opcodes prefixed with DDh or FDh make the CPU ignore the prefix, |
| As a result, the byte that immediately follows the prefix is treated as  |
| the first byte of a new instruction. The prefix takes 4 T-states.	   |
'=========================================================================*/

INSN(xy_illegal)
	{
	PC++;
	return insn_table[DATA[0] = DATA[1]](self);
	}


/* MARK: - Instructions: Hooking */

INSN(hook)
	{
	if (self->hook == Z_NULL)
		{
		Q_0
		PC++;
		return 4;
		}

	return ((DATA[0] = self->hook(CONTEXT, PC)) != Z80_HOOK)
		? insn_table[DATA[0]](self) : 0;
	}


/* MARK: - Public Functions */

/*-----------------------------------------------------------------------.
| On POWER-ON, the CPU zeroes PC, I and R, sets SP, IX, IY, AF, BC, DE,	 |
| HL, AF', BC', DE' and HL' to FFFFh [1, 2], resets the interrupt enable |
| flip-flops (IFF1 and IFF2) and selects interrupt mode 0 [3]. On Zilog  |
| NMOS models, F is sometimes set to FDh (NF reset) [1].		 |
|									 |
| There is no information about the initial state of MEMPTR and Q, so	 |
| they are assumed to be 0.						 |
|									 |
| References:								 |
| 1. https://baltazarstudios.com/webshare/Z80-undocumented-behavior.htm  |
| 2. https://worldofspectrum.org/forums/discussion/34574		 |
| 3. Young, Sean (2005-09-18). "Undocumented Z80 Documented, The" v0.91, |
|    p. 20.								 |
'=======================================================================*/

Z80_API void z80_power(Z80 *self, zbool state)
	{
	MEMPTR = PC = R = I = IFF1 = IFF2 = IM = Q =
	DATA[0] = HALT_LINE = INT_LINE = RESUME = REQUEST = 0;

	SP = IX = IY = AF = BC = DE = HL = AF_ = BC_ = DE_ = HL_ =
		state ? Z_UINT16(0xFFFF) : 0;
	}


/*-------------------------------------------------------------------------.
| The normal RESET zeroes PC, I, and R [1, 2, 3, 4, 5, 6], resets the	   |
| interrupt enable flip-flops (IFF1 and IFF2) [1, 2, 3, 4, 5] and selects  |
| interrupt mode 0 [1, 2, 3, 4, 7].					   |
|									   |
| References:								   |
| 1. Zilog (2016-09). "Z80 CPU User Manual" rev. 11, p. 6.		   |
| 2. SGS-Thomson (1990-01). "Z80 Microprocessor Family" 1st ed., p. 33.	   |
| 3. Brewer, Tony (2014-12). "Z80 Special Reset".			   |
|     * http://primrosebank.net/computers/z80/z80_special_reset.htm	   |
| 4. Flammenkamp, Achim. "Interrupt Behaviour of the Z80 CPU".		   |
|     * http://z80.info/interrup.htm					   |
| 5. https://baltazarstudios.com/webshare/Z80-undocumented-behavior.htm	   |
| 6. https://worldofspectrum.org/forums/discussion/34574		   |
| 7. Zilog (1978-05). "Z80 Family Program Interrupt Structure, The", p. 8. |
'=========================================================================*/

Z80_API void z80_instant_reset(Z80 *self)
	{
	if (HALT_LINE) {EXIT_HALT;}

	PC = R = I = IFF1 = IFF2 = IM =
	DATA[0] = HALT_LINE = RESUME = REQUEST = 0;
	}


#ifdef Z80_WITH_SPECIAL_RESET
	Z80_API void z80_special_reset(Z80 *self)
		{REQUEST |= Z80_REQUEST_SPECIAL_RESET;}
#endif


Z80_API void z80_int(Z80 *self, zbool state)
	{
	if (!(INT_LINE = state)) REQUEST &= ~(zuint8)Z80_REQUEST_INT;
	else if (IFF1) REQUEST |= Z80_REQUEST_INT;
	}


Z80_API void z80_nmi(Z80 *self)
	{REQUEST |= Z80_REQUEST_NMI;}


#ifdef Z80_WITH_EXECUTE
	Z80_API zusize z80_execute(Z80 *self, zusize cycles)
		{
		ZInt16 *xy;

		R7		  = R;
		self->cycles	  = 0;
		self->cycle_limit = cycles;

		if (RESUME && cycles) switch (RESUME)
			{
			case Z80_RESUME_HALT:
			(void)halt(self);
			break;

			case Z80_RESUME_XY:
			RESUME = 0;
			R++;
			XY = (xy = &self->ix_iy[(DATA[0] >> 5) & 1])->uint16_value;
			self->cycles += xy_insn_table[DATA[1] = FETCH_OPCODE(PC + 1)](self);
			xy->uint16_value = XY;
			break;
			}

		while (self->cycles < self->cycle_limit)
			{
			R++;
			self->cycles += insn_table[DATA[0] = FETCH_OPCODE(PC)](self);
			}

		R = R_ALL;
		return self->cycles;
		}
#endif


Z80_API zusize z80_run(Z80 *self, zusize cycles)
	{
	ZInt16 *xy;
	zuint8 ird;

	/*---------------------------------------------------------------------.
	| The CPU increments R during each M1 cycle without altering the most  |
	| significant bit, commonly known as R7. This behavior is not emulated |
	| in every increment for obvious speed reasons. Instead, a copy of R   |
	| is used to preserve R7, which is restored before returning from this |
	| function. The emulation of `ld {a,r|r,a}` takes this into account.   |
	'=====================================================================*/
	R7 = R;

	self->cycles	  = 0;
	self->cycle_limit = cycles;

	if (RESUME && cycles) switch (RESUME)
		{
		/*------------------------------------------------------------.
		| The CPU is halted. To avoid affecting the speed of the main |
		| execution loop, this state is executed by a dedicated loop  |
		| in the function that emulates the `halt` instruction.	      |
		'============================================================*/
		case Z80_RESUME_HALT:
		if (REQUEST)
			{
			RESUME = 0;

#			ifdef Z80_WITH_SPECIAL_RESET
				if ((REQUEST & Z80_REQUEST_SPECIAL_RESET) && HALT_LINE)
					{
					zuint8 opcode;

					HALT_LINE = 0;

					if (self->halt != Z_NULL)
						self->halt(CONTEXT, Z80_HALT_EXIT_EARLY);

					if (IS_XY_PREFIX(DATA[0] = opcode = DATA[2]))
						self->cycles += insn_table[FETCH_OPCODE(PC)](self);

					else if (opcode != 0x76)
						{
						PC--;
						self->cycles += insn_table[opcode](self) - 4;
						}
					}
#			endif
			}

		else (void)halt(self);
		break;

		/*--------------------------------------------------------------.
		| The CPU is in normal operation state; the emulator ran out of |
		| clock cycles by fetching a prefix DDh or FDh.			|
		'==============================================================*/
		case Z80_RESUME_XY:
		RESUME = 0;
		R++;
		XY = (xy = &self->ix_iy[(DATA[0] >> 5) & 1])->uint16_value;
		self->cycles += xy_insn_table[DATA[1] = FETCH_OPCODE(PC + 1)](self);
		xy->uint16_value = XY;
		break;

		/*----------------------------------------------------------------.
		| The CPU is responding to an INT in mode 0; the emulator ran out |
		| of clock cycles by fetching a prefix DDh or FDh.		  |
		'================================================================*/
#		ifdef Z80_WITH_FULL_IM0
			case Z80_RESUME_IM0_XY:
			ird = DATA[0];
			goto im0_begin;
#		endif
		}

	while (self->cycles < self->cycle_limit) /* main execution loop */
		{
		if (REQUEST)
			{
			/*-------------------------------------------------------------------------.
			| After detecting a special RESET signal, the CPU completes the ongoing	   |
			| instruction or interrupt response and then zeroes PC during the falling  |
			| edge of the next M1T1. The special RESET can be used in conjunction with |
			| an interrupt, in which case PC is zeroed during the subsequent interrupt |
			| acknowledge M-cycle. Otherwise, if no interrupt has been accepted at the |
			| TLAST of the instruction or interrupt response in which the special	   |
			| RESET has been detected, the CPU produces an internal NOP of 4 T-states  |
			| to allow for the fetch-execute overlap to take place, during which it	   |
			| fetches the next opcode and zeroes PC.				   |
			|									   |
			| References:								   |
			| * Brewer, Tony (2014-12). "Z80 Special Reset".			   |
			|     * http://primrosebank.net/computers/z80/z80_special_reset.htm	   |
			| * US Patent 4486827.							   |
			| * Checked with "Visual Z80 Remix".					   |
			'=========================================================================*/
#			ifdef Z80_WITH_SPECIAL_RESET
				zuint8 special_reset = REQUEST & Z80_REQUEST_SPECIAL_RESET;
#			endif

			/*-------------------------------------------------------------------------.
			| NMI Response: Execute `rst 66h`			| T-states: 11:533 |
			|--------------------------------------------------------------------------|
			| The non-maskable interrupt takes priority over the maskable interrupt	   |
			| and cannot be disabled under software control. Its usual function is to  |
			| provide immediate response to important signals. The CPU responds to an  |
			| NMI by pushing PC onto the stack and jumping to the ISR located at	   |
			| address 0066h. The interrupt enable flip-flop #1 (IFF1) is reset to	   |
			| prevent any maskable interrupt from being accepted during the execution  |
			| of this routine, which is usually exited by using a `reti` or `retn`	   |
			| instruction to restore the original state of IFF1 [1].		   |
			|									   |
			| Some technical documents from Zilog include an erroneous timing diagram  |
			| showing an NMI acknowledge cycle of 4 T-states. However, documents from  |
			| other manufacturers and third parties specify that this M-cycle has 5	   |
			| T-states, as has been confirmed by low-level tests [2] and electronic	   |
			| simulations [3].							   |
			|									   |
			| In 2022, Manuel Sainz de Baranda y Goñi discovered that the CPU does not |
			| accept a second NMI during the NMI response [4, 5]. Therefore, it is not |
			| possible to chain two NMI responses in a row without executing at least  |
			| one instruction between them [3].					   |
			|									   |
			| References:								   |
			| 1. Zilog (1978-05). "Z80 Family Program Interrupt Structure, The",	   |
			|    pp. 4-5.								   |
			| 2. https://baltazarstudios.com/webshare/Z80-undocumented-behavior.htm	   |
			| 3. Checked with "Visual Z80 Remix".					   |
			| 4. https://spectrumcomputing.co.uk/forums/viewtopic.php?p=91405#p91405   |
			| 5. https://stardot.org.uk/forums/viewtopic.php?p=356579#p356579	   |
			'=========================================================================*/
			if (REQUEST & Z80_REQUEST_REJECT_NMI)
				REQUEST = 0;

			else if (REQUEST & Z80_REQUEST_NMI)
				{
				REQUEST = Z80_REQUEST_REJECT_NMI;
				IFF1 = 0;
				if (HALT_LINE) {EXIT_HALT;}
				R++;
				if (self->nmia != Z_NULL) (void)self->nmia(CONTEXT, PC);
				DATA[0] = 0;
				Q_0

#				ifdef Z80_WITH_SPECIAL_RESET
					PUSH(PC >> special_reset);
#				else
					PUSH(PC);
#				endif

				MEMPTR = PC = 0x66;
				self->cycles += 11;
				continue;
				}

			/*-------------------------------------------------------------------------.
			| INT Response								   |
			|--------------------------------------------------------------------------|
			| The maskable interrupt is enabled and disabled by using, respectively,   |
			| the instructions `ei` and `di`, which control the state of the interrupt |
			| enable flip-flops (IFF1 and IFF2). The CPU does not accept this kind of  |
			| interrupt during an `ei` instruction. This allows ISRs to return without |
			| the danger of being interrupted immediately after re-enabling interrupts |
			| if the /INT line is still active, which could cause a stack overflow.	   |
			|									   |
			| In 2021, Andre Weissflog (aka Floh) discovered that `reti` and `retn` do |
			| not accept the maskable interrupt if IFF1 and IFF2 do not have the same  |
			| state prior to the execution of the instruction, which can only be	   |
			| caused by an earlier NMI response [1]. This behavior was rediscovered in |
			| 2022 by Manuel Sainz de Baranda y Goñi [2, 3].			   |
			|									   |
			| References:								   |
			| 1. Weissflog, Andre (2021-12-17). "New Cycle-Stepped Z80 Emulator, A".   |
			|     * https://floooh.github.io/2021/12/17/cycle-stepped-z80.html	   |
			| 2. https://spectrumcomputing.co.uk/forums/viewtopic.php?t=7086	   |
			| 3. https://stardot.org.uk/forums/viewtopic.php?t=24662		   |
			'=========================================================================*/
			else if (
#				ifdef Z80_WITH_SPECIAL_RESET
					(REQUEST & Z80_REQUEST_INT) &&
#				endif
				/* If the previous instruction is not `ei` and... */
				DATA[0] != 0xFB &&
				/* the previous instruction is not `reti/retn`,
				   or IFF1 has not changed. */
				(self->data.uint32_value & Z_UINT32_BIG_ENDIAN(Z_UINT32(0xFFC70100)))
				!=			   Z_UINT32_BIG_ENDIAN(Z_UINT32(0xED450000))
			)
				{
#				ifdef Z80_WITH_FULL_IM0
					Z80Read hook;
					IM0 im0;
#				endif

				REQUEST = IFF1 = IFF2 = 0;
				if (HALT_LINE) {EXIT_HALT;}

				/*----------------------------------------------------------------------.
				| Due to a bug, the Zilog Z80 NMOS resets PF when an INT is accepted	|
				| during the execution of the `ld a,{i|r}` instructions.		|
				|									|
				| References:								|
				| * Zilog (1989-01). "Z80 Family Data Book", pp. 412-413.		|
				| * Roshchin, Ivan (1998). "Undocumented Feature of the Z80 Processor". |
				|     * https://zxpress.ru/article.php?id=7820				|
				|     * http://code-zx.zxnet-archive.ru/id/123456			|
				'======================================================================*/
#				ifdef Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG
					if (	(OPTIONS & Z80_OPTION_LD_A_IR_BUG) &&
						(self->data.uint16_array[0] & Z_UINT16_BIG_ENDIAN(Z_UINT16(0xFFF7)))
						==			      Z_UINT16_BIG_ENDIAN(Z_UINT16(0xED57))
					)
						FLAGS = F & ~(zuint8)PF;
#				endif

				/*---------------------------------------------------------------------.
				| The INT acknowledge cycle (INTA) indicates that the interrupting I/O |
				| device can write to the data bus. 2 wait T-states are automatically  |
				| added to this M-cycle, allowing sufficient time to identify which    |
				| device must insert the interrupt response data (IRD). The first and  |
				| possibly sole byte of the IRD is read from the data bus during this  |
				| special M1 cycle.						       |
				|								       |
				| The value FFh is assumed when the `Z80::inta` callback is not used.  |
				| This is the most convenient default IRD, since an `rst 38h` will be  |
				| executed if the interrupt mode is 0.				       |
				'=====================================================================*/
				R++;
				ird = (self->inta != Z_NULL) ? self->inta(CONTEXT, PC) : 0xFF;

#				ifdef Z80_WITH_SPECIAL_RESET
					PC >>= special_reset;
#				endif

				switch (IM)
					{
					/*-------------------------------------------------------------------------.
					| Interrupt Mode 0: Execute Instruction	     | T-states: 2*n + instruction |
					|--------------------------------------------------------------------------|
					| An instruction supplied via the data bus is executed. Its first byte is  |
					| read during the INT acknowledge cycle (INTA). If it is an opcode prefix, |
					| additional M-cycles of this kind are produced until the final opcode of  |
					| the instruction is fetched [1]. Each INTA M-cycle takes as many T-states |
					| as its normal M1 counterpart (the opcode fetch M-cycle) plus the 2 wait  |
					| T-states mentioned above [1]. Subsequent bytes of the instruction are	   |
					| fetched by using normal memory read M-cycles [1, 2], during which the	   |
					| interrupting I/O device must still supply the data [2]. The PC register, |
					| however, remains at its pre-interrupt state, not being incremented as a  |
					| result of the instruction fetch [1, 2].				   |
					|									   |
					| References:								   |
					| 1. Checked with "Visual Z80 Remix".					   |
					| 2. Zilog (1978-05). "Z80 Family Program Interrupt Structure, The",	   |
					|    pp. 6, 8.								   |
					'=========================================================================*/
					case 0:
					DATA[0] = ird;

#					ifdef Z80_WITH_FULL_IM0
						im0_begin:

						/*-----------------------------------------------------.
						| The `Z80::hook` callback is temporarily disabled, as |
						| traps are ignored during the INT response in mode 0. |
						'=====================================================*/
						hook	   = self->hook;
						self->hook = Z_NULL;

						/*------------------------------------------------------------------------.
						| The `Z80::fetch` callback is temporarily replaced by a trampoline that  |
						| invokes `Z80::int_fetch`. This trampoline needs to access the callback  |
						| pointer in addition to the initial, non-incremented value of PC, so the |
						| value of `Z80::context` is temporarily replaced by a pointer to an	  |
						| `IM0` object that holds the real context and all this data, which also  |
						| makes it necessary to replace other callbacks with trampolines so that  |
						| the real context can be passed to them.				  |
						|									  |
						| The main idea here is that the instruction code will invoke trampolines |
						| rather than callbacks, and the one assigned to `Z80::fetch` will ignore |
						| the received fetch address, passing instead to `Z80::int_fetch` the	  |
						| initial, non-incremented value of PC.					  |
						'========================================================================*/
						im0.z80	      = self;
						im0.context   = CONTEXT;
						im0.fetch     = self->fetch;
						im0.read      = self->read;
						im0.write     = self->write;
						im0.in	      = self->in;
						im0.out	      = self->out;
						im0.pc	      = PC;
						self->context = &im0;
						self->fetch   = (Z80Read )im0_fetch;
						self->read    = (Z80Read )im0_read;
						self->write   = (Z80Write)im0_write;
						self->in      = (Z80Read )im0_in;
						self->out     = (Z80Write)im0_out;

						im0_execute:

						/*------------------------------------------------------------------------.
						| `call`, `djnz`, `jr` and `rst` increment PC before pushing it onto the  |
						| stack or using it as the base address. This makes it necessary to	  |
						| decrement PC before executing any of these instructions so that the	  |
						| final address is correct. `jmp` and `ret` are handled here too because  |
						| in their case this pre-decrement has no effect and PC must also not be  |
						| corrected after executing the instruction. These groups of instructions |
						| are identified by using a table of decrements. Note that `jmp (XY)` and |
						| `reti/retn` are prefixed and will be handled later.			  |
						'========================================================================*/
						if (im0_pc_decrement_table[ird])
							{
							PC -= im0_pc_decrement_table[ird];
							self->cycles += 2 + insn_table[ird](self);
							}

						/* `halt` */
						else if (ird == 0x76) HALT_LINE = 1;

						/*---------------------------------------------------------------.
						| Instructions with the CBh prefix are called directly from here |
						| after fetching the opcode in the 2nd INTA. This bypasses the   |
						| `cb_prefix` function, so PC is never incremented.		 |
						'===============================================================*/
						else if (ird == 0xCB)
							{
							R++;
							self->cycles += 4 + cb_insn_table[DATA[1] = self->inta(im0.context, im0.pc)](self);
							}

						/* Instructions with the EDh prefix. */
						else if (ird == 0xED)
							{
							Insn insn;

							R++;

							if ((insn = ed_insn_table[DATA[1] = ird = self->inta(im0.context, im0.pc)]) != ed_illegal)
								{
								im0.ld_i_a   = self->ld_i_a;
								im0.ld_r_a   = self->ld_r_a;
								im0.reti     = self->reti;
								im0.retn     = self->retn;
								self->ld_i_a = (Z80Notify)im0_ld_i_a;
								self->ld_r_a = (Z80Notify)im0_ld_r_a;

#								ifdef Z80_WITH_IM0_RETX_NOTIFICATIONS
									self->reti = (Z80Notify)im0_reti;
									self->retn = (Z80Notify)im0_retn;
#								else
									self->reti = Z_NULL;
									self->retn = Z_NULL;
#								endif

								PC -= ((ird & 0xC7) == 0x43)
									? 4 /* `ld SS,(WORD)` and `ld (WORD),SS`. */
									: 2 /* All other instructions. */;

								self->cycles += 4 + insn(self);

								self->ld_i_a = im0.ld_i_a;
								self->ld_r_a = im0.ld_r_a;
								self->reti   = im0.reti;
								self->retn   = im0.retn;
								}

							else if (self->illegal == Z_NULL)
								self->cycles += 4 + 8;

							else	{
								DATA[2] = 4;
								self->cycles += 4 + self->illegal(self, ird);
								}
							}

						/* Instructions with the prefix DDh, FDh, DDCBh or FDCBh. */
						else if (IS_XY_PREFIX(ird))
							{
							Insn insn;

							if (RESUME) RESUME = 0;

							else	{
								im0_advance_xy:
								if ((self->cycles += 6) >= self->cycle_limit)
									{
									RESUME = Z80_RESUME_IM0_XY;
									goto im0_finalize;
									}
								}

							R++;

							if (IS_XY_PREFIX(ird = self->inta(im0.context, im0.pc)))
								{
								DATA[0] = ird;
								goto im0_advance_xy;
								}

							if ((insn = xy_insn_table[ird]) == xy_illegal)
								{
								DATA[0] = ird;
								PC++;
								goto im0_execute;
								}

							XY = (xy = &self->ix_iy[((DATA[1] = ird) >> 5) & 1])->uint16_value;
							self->cycles += 2 + insn(self);
							xy->uint16_value = XY;

							/* Restore PC, except for `jp (XY)`. */
							if (ird != 0xE9) PC = im0.pc;
							}

						else	{
							self->cycles += 2 + insn_table[ird](self);
							PC = im0.pc;
							}

						im0_finalize:
						self->context = im0.context;
						self->fetch   = im0.fetch;
						self->read    = im0.read;
						self->write   = im0.write;
						self->in      = im0.in;
						self->out     = im0.out;
						self->hook    = hook;

						if (HALT_LINE)
							{
							if (self->halt != Z_NULL) self->halt(im0.context, 1);
							RESUME = Z80_RESUME_HALT;
							Q_0
							self->cycles += 6;
							(void)halt(self);
							}

						continue;

#					else
						switch (ird)
							{
							case 0xC3: /* `jp WORD` */
							Q_0
							MEMPTR = PC = int_fetch_16(self);
							self->cycles += 2 + 10;
							continue;

							case 0xCD: /* `call WORD` */
							Q_0
							MEMPTR = int_fetch_16(self);
							PUSH(PC);
							PC = MEMPTR;
							self->cycles += 2 + 17;
							continue;

							default: /* `rst N` is assumed for all other instructions. */
							Q_0
							PUSH(PC);
							MEMPTR = PC = ird & 56;
							self->cycles += 2 + 11;
							continue;
							}
#					endif

					/*----------------------------------------------------------.
					| Interrupt Mode 1: Execute `rst 38h`	 | T-states: 13:733 |
					|-----------------------------------------------------------|
					| An internal `rst 38h` is executed. The interrupt response |
					| data read from the data bus is disregarded.		    |
					'==========================================================*/
					case 1:
					DATA[0] = 0;
					Q_0
					PUSH(PC);
					MEMPTR = PC = 0x38;
					self->cycles += 13;
					continue;

					/*---------------------------------------------------------------------.
					| Interrupt Mode 2: Execute `call (i:BYTE)`	  | T-states: 19:73333 |
					|----------------------------------------------------------------------|
					| An indirect call is executed. The pointer to the ISR is loaded from  |
					| the memory address formed by taking the I register as the most       |
					| significant byte, and the interrupt response vector (IRD) read from  |
					| the data bus as the least significant byte.			       |
					|								       |
					| Zilog's official documentation states that the least significant bit |
					| of the interrupt response vector "must be a zero", since the address |
					| formed "is used to get two adjacent bytes to form a complete 16-bit  |
					| service routine starting address and the addresses must always start |
					| in even locations" [1]. However, Sean Young's experiments confirmed  |
					| that there is no such limitation [2]; any vector works regardless of |
					| whether it is even or odd.					       |
					|								       |
					| References:							       |
					| 1. Zilog (2005-03). "Z80 CPU User Manual" rev. 5, pp. 25-26.	       |
					| 2. Young, Sean (2005-09-18). "Undocumented Z80 Documented, The"      |
					|    v0.91, p. 20.						       |
					'=====================================================================*/
					case 2:
					DATA[0] = 0;
					Q_0
					PUSH(PC);
					MEMPTR = PC = READ_16((zuint16)(((zuint16)I << 8) | ird));
					self->cycles += 19;
					continue;
					}
				}

#			ifdef Z80_WITH_SPECIAL_RESET
				if (special_reset)
					{
					REQUEST = 0;

					/*---------------------------------------------------------.
					| The /HALT line goes low and then high during TLAST if a  |
					| special RESET is detected during the `halt` instruction. |
					'=========================================================*/
					if (DATA[0] == 0x76 && self->halt != Z_NULL)
						self->halt(CONTEXT, Z80_HALT_CANCEL);

					R++;
					if (self->nop != Z_NULL) (void)self->nop(CONTEXT, PC);
					DATA[0] = 0;
					Q_0;
					PC = 0;
					self->cycles += 4;
					continue;
					}
#			endif
			}

		R++;
		self->cycles = insn_table[DATA[0] = FETCH_OPCODE(PC)](self) + self->cycles;
		}

	R = R_ALL; /* Restore R7 bit. */
	return self->cycles;
	}


#ifdef Z80_WITH_DLL_MAIN_CRT_STARTUP
	int Z_MICROSOFT_STD_CALL _DllMainCRTStartup(void *hDllHandle, unsigned long dwReason, void *lpReserved)
		{return 1;}
#endif


/* Z80.c EOF */
