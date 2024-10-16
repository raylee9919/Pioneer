/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

u32 random_table[] = {
    0X785FD252, 0XC76E9A98, 0X3D9A5C28, 0XDDF7F80D, 0XDDA5C366, 0XF50D03B6, 0X1CEE0954, 0XCFDD36C9,
    0XF0914186, 0X91B2B160, 0X367BB303, 0X9C4377DE, 0X9C1D475B, 0XA7C9D2C8, 0XC90015E2, 0X3F8D6706,
    0X3C1B023A, 0X315D117E, 0XD1386694, 0XB2D985C8, 0XA875C6D5, 0X234CC6C6, 0X789A740C, 0X154E434C,
    0X33F18ADC, 0XB7F92C6C, 0XD1F89347, 0X6EC41192, 0X10D27158, 0X407C2F47, 0X166E1AB7, 0XB6BA6F3F,
    0X11DAE852, 0XDBA0C980, 0X0CDCF6CF, 0X0B1EBE7C, 0XDDB8F318, 0XD3EB0300, 0XA5838B77, 0XCF6AFB18,
    0X67FC5F10, 0XB998A042, 0XD8A2FEC7, 0X4F8F9B46, 0XC578BEAD, 0X09018EF2, 0XFF9FAB4D, 0X3C2B0E33,
    0XC3ABA426, 0X84B71325, 0X63416E21, 0XA22BFBC8, 0X56FD2D01, 0X2FDDD2F0, 0XED5030A6, 0X35FEC129,
    0X61221786, 0X4AC6F012, 0X0A492686, 0XCBDEE32F, 0X3D1A318B, 0X9DB9E24D, 0XF23658D2, 0X69E77E6E,
    0X7942D42E, 0X38F58426, 0XD8D30570, 0X3E49656F, 0XF76DED48, 0XC9085F05, 0XE9BBA6B4, 0X6DF18872,
    0XF3CC26E8, 0X4D92CBD6, 0X9C78DF10, 0X5BA57E9B, 0X4131ED12, 0X9937EDDD, 0X727D08ED, 0X8A03F094,
    0X02EA51FF, 0XBEEADF29, 0XAE11AE04, 0X241F1A1E, 0X06C6305B, 0X2AA29A52, 0XC9D8FF5E, 0X378CE175,
    0X1334354F, 0X04630BBB, 0X25C5DEA7, 0X14116009, 0XA03F67ED, 0XC06F89A0, 0X266B90C6, 0XBB6AE08B,
    0X002F7154, 0X2AE41F3C, 0X3E7AF1D7, 0XF97C3C79, 0X5435E301, 0X1DA5C861, 0XD5371D3D, 0XBCCB153F,
    0X551F2DE7, 0XFC386362, 0X4755BD0A, 0X19CE6229, 0X97FB7E8E, 0X507DE910, 0X505273AC, 0X2134A5EE,
    0X2924C23D, 0XAFF217BB, 0X4AF3E845, 0X6E28E64E, 0XD0F5BB56, 0XCCDC20EE, 0X628F4D17, 0X4DF4015F,
    0X3C808E2D, 0X9466DC70, 0XCA79F742, 0X08A43067, 0X37076A3F, 0X4F71A490, 0X389DFF15, 0XA71B3E85,
    0X72C1FDB1, 0X2B6C379F, 0XAD48BF63, 0XB68D034C, 0X13EB8661, 0X2383BE0D, 0X12563634, 0X45A29C88,
    0X9BCD375E, 0X11B1B727, 0X0784885D, 0X94790C2F, 0X502B3A1E, 0X4C64AAD5, 0X44734920, 0X69B5159F,
    0XDCAC7D05, 0XD6F3BD1A, 0XA17FCB8E, 0X92D34F73, 0X92FD8E85, 0X1C5EAD0C, 0X522BB399, 0X6518B691,
    0X250F5168, 0X71B65611, 0X59E1193D, 0X909B2DA9, 0X36015D8D, 0X379DFA14, 0X19603D65, 0XCF51EB1E,
    0X5DD28EE4, 0XF65A5CEE, 0XCD0539FA, 0X60BB1194, 0X887BBD60, 0XD9BABCE1, 0X30574E16, 0X6BDBDD88,
    0XA5AFC7E2, 0X44249B17, 0XA8BC1B39, 0XD1AE6492, 0X53C0D970, 0XF8E4E5AB, 0X6607FC13, 0X86F60220,
    0XBBA1113F, 0X2BA80FC7, 0X4987121E, 0X65F8C06C, 0X5A800D70, 0X73FE5DCE, 0XEE3D6972, 0XDFEC8B27,
    0X3DB60BDE, 0X341F767C, 0XF1119B90, 0X81FD707D, 0X8AB2631E, 0X3F3546EE, 0X8D01D53B, 0X90436C5A,
    0XC4295A7D, 0XAF65FAC4, 0X108568B4, 0X5D9BCFA6, 0XC66F2756, 0X120D75CC, 0XC45D9B22, 0X343B369F,
    0X792EB4C4, 0X6C00496A, 0X290D94AD, 0XFD965CB6, 0X0C302CF6, 0X509240FC, 0X97AF46B6, 0X8A2D71BC,
    0XAE4CCBA5, 0X1AA70FE4, 0X5F6033CD, 0X79F97583, 0X83CB1C1E, 0XF36C4014, 0X60831C6A, 0XD6867E49,
    0XA48A8AB6, 0X68F5BF64, 0X8DD86E93, 0XF57A2FED, 0XA8E1BA73, 0X2A939F59, 0X38EE1746, 0XED79169F,
    0X60E14998, 0X76BF9711, 0X817AF2EF, 0X663BE3F0, 0X1129A82B, 0XABD37314, 0X8FC23490, 0X0EB96F8F,
    0XC6FEC5E3, 0X6C20293A, 0X5833AF8A, 0XB07A79CE, 0XFB3B5730, 0X0B8A6D7F, 0X9E77D767, 0XE9C907E6,
    0X32B4CCB4, 0X1A10BE17, 0X8480F2EC, 0X831BA934, 0X18A55591, 0X36905842, 0XDD514FD6, 0X0908EDA8,
    0X139F7731, 0XC0572410, 0XF1F92336, 0X07619940, 0X3FA25ECD, 0X83254CED, 0XB9CD1059, 0X9644FFA9,
    0X3ACBD42E, 0X228EEECA, 0X2B26E73E, 0XD6140F9E, 0X948514FE, 0X80B62EEE, 0XF441F8C5, 0XB04271B2,
    0X6793CDF7, 0XB80F1C28, 0X7E905885, 0XB9ACA794, 0X9CB3249F, 0X789E5D2F, 0X9B262E6E, 0X1E6A7038,
    0X3B380043, 0XBC401577, 0X4D230264, 0X7C4BBA7A, 0XCAA1EF3F, 0X97E4C9F9, 0X274C2A86, 0X268F50EE,
    0X309EE904, 0XEAC36015, 0XA3FA8325, 0XA6D23936, 0X28C937FA, 0XE062D8E7, 0X7DB30917, 0XBD806E0B,
    0X7AFA1D79, 0XE4D8E84D, 0X597036B9, 0X1EF1D156, 0X2A5B0FB3, 0XCE991EF5, 0X2028F4DD, 0X50C9B3B5,
    0X97C12B3B, 0XD23BBAE0, 0X1B960D65, 0XF31D8313, 0XE3017E03, 0XCD04AB61, 0X90C53CE1, 0X94D47183,
    0X14EBA783, 0X82E2708E, 0XD2236628, 0XF8341014, 0X993B7F86, 0XA9630219, 0X35C684D8, 0X5CECEE65,
    0XC5385C16, 0X76F00939, 0X1D2E81BB, 0X7A8F818F, 0XA2873B96, 0X5781AE9D, 0X46C594AC, 0X7A2BF9BD,
    0X2BB1BF64, 0XC717F0E5, 0X44C71498, 0X39333482, 0XD0D916A3, 0X91D00FE7, 0X6A426981, 0X61FBF541,
    0XCC39781A, 0X184557AA, 0X3EF0E79A, 0X73B79825, 0X91A613A9, 0X50469C68, 0X2C32B42F, 0XB1C43919,
    0X07485920, 0XD1EB8F65, 0XD52C5C66, 0X4AB8B52D, 0X2D2CC29D, 0XF140D71B, 0X57EF2E9C, 0XB0834368,
    0X16E90A24, 0X0CDB4A47, 0X4FAEDD10, 0XA8E1A168, 0X2755212F, 0XD5FCED34, 0X2A0F2A27, 0XC9970821,
    0X8F4B2895, 0X76C03D4D, 0XF3F90B5B, 0XC60DDD36, 0X88DDD58D, 0XC1CC0F87, 0X17764B9E, 0X8B4BF464,
    0XFA823440, 0XA239F452, 0X37787EC4, 0X4D9B758D, 0XF75A15CD, 0XFD4CF177, 0XD95100CB, 0X93B0C982,
    0X8379E691, 0XBE0AB208, 0X6DC4FEAC, 0X22AEEA1F, 0X4B850F70, 0XB8F8CD62, 0X0DEBDF1A, 0X269E1429,
    0X29659AE4, 0X1B04B196, 0X21DDFA58, 0XF7B1B99E, 0X760BF34F, 0X5F178B91, 0X59FECCA9, 0XE5139A62,
    0XCC36EC05, 0XC2068921, 0XCEDF6473, 0X06EB34A5, 0X80AE41B4, 0X7AEBB8F4, 0X6104E4A8, 0X55FBC7F5,
    0XE817C755, 0X6AB0C5E7, 0X5D9172B1, 0X162C5D24, 0X22945619, 0X04350EF5, 0XFDB182FD, 0X3E2540EB,
    0XB4A0BB27, 0XDFD58FD5, 0X91C4441F, 0X84A25E48, 0XE457A46C, 0X621738EF, 0XC1B7FF52, 0XA10EFEF8,
    0X217E8104, 0X49748A08, 0X23ACBA97, 0XD1F9942D, 0X6256ACCB, 0X142B933C, 0X10B98787, 0XCB33142E,
    0X19AC06E1, 0X52CEE08E, 0X8D77632E, 0XDA03F7D1, 0X578DE2A6, 0X3BD2F9EA, 0XC0C813AB, 0X25D7107E,
    0X86FD62E5, 0X8261FCF0, 0X18C4AD67, 0XDF5D8789, 0X7727EAC5, 0X2106E468, 0X39BFD958, 0XDAB7A36B,
    0XBF258ACA, 0X8338D680, 0X5B864793, 0XBD7BDB6F, 0XF4F31623, 0X29AC72E3, 0XDA29CDFC, 0XAABE7C41,
    0X0203E200, 0X5A1EEB8F, 0X9848F149, 0X05F11263, 0X37D9625A, 0X60783D70, 0X6739C04E, 0X1A0D4336,
    0X9F031257, 0XDA1F4A49, 0X41A6324E, 0X51EC1D6A, 0XAB4148F7, 0X2F9B8C80, 0X14A0D854, 0X931552F4,
    0X71330E17, 0XD6E9D265, 0X3AD188FC, 0X93281DF6, 0X658D6D96, 0X4E1738CC, 0XBE5F2EE5, 0XF2C70BEC,
    0XC2FAD62B, 0X0104FF1B, 0X6ED9E32F, 0XB8E8984B, 0XF93F54AA, 0XAAEF1FB8, 0XCA156DB3, 0X8D9DEDB8,
    0XDF453438, 0X0D95CC43, 0X5D2440BB, 0X82588498, 0XC31728A6, 0X34C053BF, 0X259AD9E0, 0XC926334F,
    0XFE4C6CE3, 0X348482B4, 0X358A70B9, 0X0DA8F365, 0XEA3F83D0, 0XB37F271C, 0X8AB0988A, 0X51F55D4B,
    0XF297323E, 0X8025C100, 0X1970504E, 0X07DB1543, 0X42AAADC8, 0X36740E65, 0X6D6E3642, 0XB1727DAE,
    0X64A50D11, 0X44744492, 0X36ED4A9A, 0X1A6AE61F, 0X957EF091, 0X06379D24, 0X2AD94A57, 0XD07FD7B5,
    0XAB1FBB6C, 0XBB5D3040, 0X8CD38588, 0X8D5C692F, 0X9F0535ED, 0X5F626647, 0XF4E8D9FD, 0X4A27C0DE,
    0X39588911, 0X382F9FAD, 0X4518AEB1, 0XC2A427E5, 0X25C1E6FD, 0XB2C67D7B, 0XC072C15F, 0X51A496DE,
    0X8FC607DF, 0X8543EDE5, 0XD9CE3F58, 0X55FC3BF8, 0XE7BA0EF9, 0XFE36F2C3, 0XE2C65C8A, 0X25AA0975,
    0X811DCAA3, 0X91A3434F, 0X360F5E36, 0X47EF8B00, 0X225506D1, 0XC58E83BD, 0X2405E7D3, 0XD1E74111,
    0XEF5D51F2, 0X2A42E11D, 0X51768BB6, 0X9494C322, 0XC096AE2C, 0X1EFFA4EA, 0X185732F0, 0XC51444C9,
    0XD5EF9D82, 0X0E122378, 0X109C8B2F, 0XB613E22F, 0XBA124497, 0XA2DD53D9, 0X92D1944E, 0XFFB7D659,
    0XE287C328, 0XA73A02F8, 0XB8BA0541, 0X448FB6AB, 0XE065DE10, 0XA008B8E1, 0X95E61229, 0X5F08D96A,
    0X408ACE7A, 0X38C6E429, 0X92EF0C91, 0XF53BA5F9, 0X799B904B, 0X99AE00ED, 0X875F12A1, 0X3F108896,
    0X85A564A4, 0XB46319F2, 0XCA6C9314, 0XE02C6BD2, 0X15BE02C6, 0X8104CB74, 0XF5CB52CF, 0X8DF9CBDB,
    0X05CE69FA, 0X5925D005, 0X8203B59E, 0X9196C3D3, 0XDD4E7A64, 0X5626D390, 0XEB94C208, 0X9E7B6CAA,
    0XE293B9FC, 0X2AA2507C, 0XAF0D084A, 0XD1186EFA, 0X4511B297, 0XE7052BD5, 0XD34DBBA6, 0XEA8D1E12,
    0X8C5C7BEB, 0X0F6EEFB5, 0XCC7FBF4C, 0XB7996A28, 0X4D674927, 0XD642610E, 0X6B4B8A0C, 0X7CF704C7,
    0X960925B6, 0X410B36D0, 0XFAB174FF, 0X8D56ADC8, 0XD5FD5F43, 0XC62B83A2, 0X448F4901, 0XE50F9192,
    0X59247EA5, 0X9E91ECBD, 0X08FE89AD, 0X70F630C4, 0XEF702A12, 0X58A3752E, 0X1D2A6440, 0X90FBBD72,
    0XABF17C8E, 0X5F0C0B18, 0X1329692B, 0XA5F444C0, 0X35C6FFBB, 0XA97AF1E7, 0X2167CF1E, 0X5AED7B3D,
    0X65265601, 0X7FF12E0B, 0X064E5F48, 0X6522EF1E, 0X1A094ABC, 0XA0880281, 0X93394FA4, 0X89A9637B,
    0X311EF9F9, 0X79540335, 0X4EF48A02, 0X34F01BE7, 0X7E21A1CE, 0XC653AB3C, 0X7471F762, 0X3C5DEC48,
    0XBB1F3F6F, 0X4B032673, 0X5ACC42EB, 0X87FA57DC, 0X56DD35E0, 0XDF1A8135, 0XEAA41429, 0X68D009C3,
    0X18CE3B97, 0X85BA4BFC, 0XA67F8E45, 0X2255AFD8, 0X1232A10C, 0X99C11731, 0X10300223, 0X660313E2,
    0X5164C7B5, 0X026213D1, 0X34FD46A1, 0X57D3A4E5, 0XABEFE092, 0XA66A127B, 0XAD724514, 0X0FA20B01,
    0X4F09E2B9, 0XCC679313, 0XA4D78C43, 0XBF46D6B0, 0X9C1F3CF3, 0X30AC3E15, 0X2DE883FA, 0X8F472DC7,
    0X72B91A3D, 0X3ACDC3D6, 0XBC107E4A, 0X14A96BF6, 0XA510D2A3, 0X80F4132C, 0XC8DC77BF, 0X25C85098,
    0XB9DF83DC, 0X29332DBF, 0X050170DA, 0XEAE142CE, 0XBD401D57, 0XE243AE08, 0XEDAC23D3, 0XA035712D,
    0X1E7BEAA0, 0X1084324D, 0X211DADBE, 0XBB5072EC, 0XC1365AD7, 0X19D8B2A7, 0X19DA2FD8, 0X34A59794,
    0X7833F559, 0X5BB278F2, 0XB3556B1B, 0X69DA5D05, 0X5792ED6B, 0X4BBAC153, 0X855802BA, 0X6D24B786,
    0X4C4D1A03, 0X38788B16, 0X6B2B11DF, 0X68D68E9C, 0X7F3DD31B, 0X222AC660, 0X7BCE200B, 0XD1794CE4,
    0X13C79D6F, 0X211AA79D, 0XA65A68ED, 0X977C3EB2, 0X6F7BCCCA, 0X59090B0D, 0X2F86CB8C, 0X2820C877,
    0X3F40F118, 0X36F9A1E1, 0X9FCFF629, 0X9D0ECA2C, 0X05F4B266, 0X1CE3192A, 0X2A788D59, 0X0DA896FD,
    0XD2E15C40, 0XDBF7E32C, 0X5FA5D877, 0X4E70263D, 0X93A7C2C3, 0XEFAD746B, 0XA9C59EB0, 0X61975FC5,
    0X3A350372, 0XDC1A8786, 0X5E4F0ED2, 0X83F71BF1, 0X69105983, 0X8880BD3F, 0X46707FC6, 0XE1C2083E,
    0XA75B9D9A, 0X4B9C70A3, 0X04362D13, 0XE40DEFBD, 0XB0C63F29, 0X359E5A36, 0X6F543E87, 0X44E0B118,
    0XB4946E77, 0X72A9AD48, 0X1F8CC8BB, 0X22E745E0, 0X7181E49F, 0X471DFAFE, 0X4A9979CC, 0X53435998,
    0X1B11F070, 0X75FBB60F, 0X93A81B4D, 0X9798832E, 0X5D2C88A8, 0XADF643D6, 0X9B87521F, 0X3AB7F6B9,
    0XBED68600, 0XA6BA7689, 0XBFC5AB5E, 0X0C3867F0, 0XA9AF5755, 0X8BE99D67, 0X99E61921, 0XDC62190F,
    0X9556C7ED, 0X82F3A2FF, 0XA53EEE84, 0X0C770F3D, 0X14C46F76, 0X2EF39121, 0X477B8C61, 0X772AF31B,
    0X3E2B85D6, 0X9D2E228D, 0X348B8085, 0X2984A35D, 0XBA246C30, 0XCDE17FC2, 0XAB105AB6, 0XA194BB88,
    0X514013A5, 0XD146153A, 0X481823CC, 0X434F19F0, 0XC19E8D61, 0X2048F600, 0XA17E38FA, 0X33200BB1,
    0X31D04067, 0XBDF53BB8, 0XE07849AD, 0XA0C69887, 0X35E47F1E, 0XB3838A8D, 0XA935A068, 0X3FD8F8A3,
    0XD5402F0A, 0XE839EE5A, 0X05091002, 0XD97D27E4, 0X37502557, 0X67C26BA6, 0X721FF336, 0X3C28DB99,
    0X3525704B, 0XEA7640C4, 0XB53BAD80, 0X1BE20581, 0X55CDF8AB, 0X63A06436, 0X0CA643FA, 0XD6F6D9AD,
    0X65EE054E, 0XCF6F9B48, 0XB73845AB, 0X10F5C1C3, 0X2831715E, 0X53A307BB, 0XC1361DDD, 0X4D7F0548,
    0X5026F606, 0X7AEF7653, 0XD74EC60E, 0X790FCD5F, 0X973B6862, 0X8AB7BDDD, 0XA3D9460A, 0XFD0B937B,
    0XDAA2DD38, 0XD46E7DA0, 0X9E5254B8, 0XF13DC2FA, 0XBACE71BD, 0X785B0CBE, 0XEFDB2143, 0X3096F73F,
    0XBFF21BCB, 0XD7819D14, 0X010B542A, 0XA2FF6AD0, 0XBD06137C, 0X7204CF37, 0XA48E0FAD, 0XBABD52A6,
    0XEB48397B, 0XE428E63A, 0XC3EC08B9, 0X5D7CE883, 0X185F3BE2, 0XEF1A8599, 0XC900CAC1, 0XF593224E,
    0X01564B67, 0X41A29451, 0X752422CD, 0X93AA4EB4, 0XC388562A, 0X8752E7DB, 0X56C74085, 0X0DE6FE77,
    0X1EB93635, 0X3B5DF77D, 0X16C75478, 0XC4CD3763, 0X1DD03D4C, 0X27085AED, 0X45408635, 0X409854D9,
    0X61AD529B, 0X5C9B55C5, 0X08FD4753, 0XD03E50BC, 0X7E23EA43, 0X520ACE7F, 0XC40BFE20, 0XD76EF6FE,
    0X78797D13, 0XA7DAF437, 0X1F285FAD, 0XBFDCA876, 0XCF126E9C, 0X63A69AE2, 0X3D0D73D4, 0X85BA5FA9,
    0XFC902F41, 0XA51BB00A, 0X105EEA02, 0XF4738E56, 0X47AB31D1, 0XE1F395B9, 0X45399717, 0X5AF0A52F,
    0XD802B4D2, 0XF5A9E52D, 0XD559337F, 0X36F25AA8, 0XEA00F11F, 0XF1C06B49, 0X7B8534EB, 0X4958629F,
    0X99E19305, 0X86D76370, 0XCA36E77D, 0XDFB13FBC, 0X900D0A3E, 0XA3B2AB90, 0X6A025B28, 0XCD105407,
    0XF1DD5DEE, 0X2D5B1258, 0XA4C0A7B7, 0XEA7893E6, 0XB119869A, 0X44C0E432, 0XD3CBDBFE, 0XF9542F5B,
    0X714BB800, 0X06598166, 0X543D8A3E, 0XF0DC9DB0, 0X617911C4, 0XD101CDAF, 0X7B10FA76, 0X919CA213,
    0X58753CB2, 0X787F9230, 0XE2F47C8C, 0X77B45792, 0X4E93F23C, 0X80946DC0, 0X66D155AE, 0X5E0091DF,
    0X08C87D0A, 0XD2E0AC2F, 0X44865280, 0X3A3B5266, 0XA28647CA, 0XBF0D4B5A, 0X1BA5B3BB, 0X912660BE,
    0X92763718, 0XCA38E9A0, 0XFF5D590A, 0XEDFF87C1, 0X938AC35B, 0X8C21A62B, 0XED5F119B, 0XFA2DB945,
    0X1F1E18EC, 0X779AED2B, 0XDF376669, 0X10A81695, 0XC14FB0CF, 0XB3E53E49, 0X0598292B, 0XEBB337FF,
    0X15FBAAB4, 0X403544A0, 0XF9A2DF81, 0XDCD01A16, 0XCB46DC3B, 0XC458E272, 0XBEF5FF55, 0XD2F5403D,
    0XB58A461A, 0XE6DED2CF, 0XB5C4928F, 0X0DC96FB9, 0X434872A9, 0X090C5930, 0X2777630B, 0X7A79B771,
    0X634CE64F, 0X94C97281, 0X29A1DF20, 0X6AE8081B, 0X2E065FE5, 0X9704989F, 0XC542926F, 0XDF64F7FD,
    0XCBCFB765, 0X3A3880CF, 0X1B7743A1, 0XFA602B73, 0X73594934, 0XB2F4DF0B, 0X06A89732, 0X96238EF4,
    0X4230800E, 0X44E9F148, 0X79D93828, 0X11CB5B0F, 0X1289789A, 0X54355502, 0X8FDB2F0E, 0X5D0E90B3,
    0X605B6386, 0X309A0CAB, 0X73E3582F, 0XA150E0A5, 0XC4A1695B, 0X1543AC0B, 0XE92DF7F1, 0X498EF7C8,
    0X0A513FD4, 0X51A7823A, 0X1AB3C467, 0XE8E73333, 0X8FAAE1AD, 0X1423205F, 0X2E06FE4F, 0X0EFF3F00,
};

u32 max_in_random_table = 0Xffb7d659; 
u32 min_in_random_table = 0x002f7154; 

struct Random_Series 
{
    u32 state;
};

inline Random_Series
seed(u32 seed) 
{
    Random_Series result = {};
    result.state = random_table[seed % array_count(random_table)];
    return result;
}

inline u32
rand_next(Random_Series *series)
{
#if 0
    u32 result = random_table[series->next_idx++];
    if (series->next_idx > array_count(random_table)) 
    {
        series->next_idx = 0;
    }
    return result;
#else
    u32 x = series->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    series->state = x;
    return x;
#endif
}

inline f32
rand_unilateral(Random_Series *series)
{
#if 0
    f32 d = (f32)(max_in_random_table - min_in_random_table);
    f32 r = (f32)rand_next(series);
    f32 t = safe_ratio(r - min_in_random_table, d);
    f32 result = lerp(0.0f, 1.0f, t);
    return result;
#else
    f32 d = (f32)(0xffffffff);
    f32 r = (f32)rand_next(series);
    f32 t = r / d;
    f32 result = lerp(0.0f, 1.0f, t);
    return result;
#endif
}

inline f32
rand_bilateral(Random_Series *series)
{
    f32 result = rand_unilateral(series) * 2.0f - 1.0f;
    return result;
}

inline f32
rand_range(Random_Series *series, f32 lo, f32 hi)
{
    f32 result = lerp(lo, rand_unilateral(series), hi);
    return result;
}
