/*
  Emon.h - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
*/

#ifndef EmonLib_h
#define EmonLib_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

// define theoretical vref calibration constant for use in readvcc()
// 1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
// override in your code with value for your specific AVR chip
// determined by procedure described under "Calibrating the internal reference voltage" at
// http://openenergymonitor.org/emon/buildingblocks/calibration
#ifndef READVCC_CALIBRATION_CONST
#define READVCC_CALIBRATION_CONST 1126400L
#endif

// to enable 12-bit ADC resolution on Arduino Due,
// include the following line in main sketch inside setup() function:
//  analogReadResolution(ADC_BITS);
// otherwise will default to 10 bits, as in regular Arduino-based boards.
#if defined(__arm__)
#define ADC_BITS    12
#elif defined(ESP32)
#define ADC_BITS    12
#else
#define ADC_BITS    10
#endif

#define ADC_COUNTS  (1<<ADC_BITS)

const int ADC_LUT[4096] = {
    0,50,53,57,60,64,65,66,67,68,70,71,72,73,74,75,76,78,79,80,81,83,84,85,87,88,90,91,92,94,95,
    97,98,99,101,102,103,105,106,108,109,110,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,
    131,132,133,134,136,137,138,139,140,141,142,143,145,146,147,149,150,151,153,154,155,157,158,159,160,161,162,162,163,164,
    164,165,166,166,167,168,168,169,170,170,171,172,172,173,174,174,175,176,185,192,193,194,195,196,197,198,198,199,200,201,
    202,203,204,205,206,206,207,208,209,211,212,213,214,215,216,217,218,219,220,222,223,224,225,227,228,229,231,232,234,235,
    237,238,239,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,262,263,264,265,266,268,269,
    270,271,272,273,275,276,277,278,279,280,281,282,283,284,285,286,288,289,290,291,292,293,294,295,296,298,299,300,301,302,
    303,304,305,306,307,308,309,310,311,312,313,314,315,316,316,317,318,319,320,322,323,324,325,327,328,329,330,332,333,334,
    335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,353,354,355,357,358,359,361,362,364,365,366,368,369,
    370,371,373,374,375,376,377,379,380,381,382,384,384,385,386,387,388,389,390,391,392,392,393,394,395,396,397,398,399,399,
    400,402,403,404,405,406,407,408,409,410,411,412,413,415,416,417,418,419,420,422,423,424,425,426,428,429,430,431,432,434,
    435,437,439,440,442,443,445,446,448,449,450,451,452,453,454,455,456,457,458,459,460,461,462,463,464,465,467,468,469,470,
    471,472,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,498,499,500,
    501,502,502,503,504,505,505,506,507,508,509,509,510,511,512,513,514,515,516,518,519,520,521,522,524,525,526,527,528,530,
    531,532,533,534,535,537,538,539,540,541,543,544,545,547,548,550,551,553,554,556,557,559,560,561,563,564,565,566,568,569,
    570,571,573,574,575,576,577,578,578,579,580,581,581,582,583,583,584,585,586,586,587,588,589,589,590,591,591,593,594,596,
    597,599,600,602,603,605,606,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,623,624,625,626,627,628,629,
    630,630,631,632,633,634,635,636,637,637,638,639,640,641,642,644,645,646,647,648,650,651,652,653,654,656,657,658,659,660,
    661,662,663,664,665,666,667,668,669,670,671,672,673,674,676,677,678,680,681,682,684,685,686,688,689,690,691,692,693,694,
    695,696,697,698,699,700,701,702,703,704,705,706,707,708,708,709,710,711,712,713,714,714,715,716,717,718,719,719,721,724,
    726,729,731,734,736,737,738,739,740,741,742,743,744,745,746,747,748,748,749,750,751,753,756,758,761,763,766,768,769,769,
    770,771,772,772,773,774,774,775,776,777,777,778,779,779,780,781,782,782,783,784,785,787,788,789,791,792,793,795,796,797,
    799,800,801,802,803,804,805,806,807,808,809,810,812,813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,829,
    830,832,833,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,850,851,852,853,853,854,855,856,856,857,
    858,859,859,860,861,862,862,863,864,866,868,870,872,874,875,877,879,881,882,882,883,884,885,886,887,888,889,890,891,892,
    893,894,895,897,898,899,900,902,903,904,905,907,908,909,910,912,913,914,915,916,917,918,919,920,921,922,923,924,925,926,
    927,928,929,931,932,934,935,936,938,939,940,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,957,959,960,961,
    962,963,964,966,967,968,969,971,972,973,974,975,977,980,982,984,986,988,991,992,993,993,994,995,995,996,996,997,998,998,
    999,999,1000,1001,1001,1002,1002,1003,1004,1004,1005,1005,1006,1007,1007,1008,1012,1016,1020,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,
    1035,1036,1037,1038,1039,1040,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1057,1059,1061,1063,1065,1067,1069,1071,
    1072,1073,1074,1075,1075,1076,1077,1078,1078,1079,1080,1081,1082,1082,1083,1084,1085,1085,1086,1087,1088,1089,1090,1091,1091,1092,1093,1094,1095,1096,
    1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1108,1109,1111,1112,1113,1115,1116,1118,1119,1120,1121,1121,1122,1123,1124,1124,1125,1126,1127,1127,
    1128,1129,1129,1130,1131,1132,1132,1133,1134,1134,1135,1136,1138,1140,1141,1143,1145,1147,1149,1151,1152,1153,1154,1156,1157,1158,1159,1160,1161,1162,
    1163,1165,1166,1167,1168,1169,1170,1171,1172,1173,1174,1175,1176,1178,1179,1180,1181,1182,1183,1184,1185,1186,1187,1188,1189,1190,1191,1192,1193,1194,
    1195,1196,1197,1198,1199,1200,1201,1203,1204,1205,1206,1207,1208,1210,1211,1212,1213,1214,1215,1216,1217,1219,1220,1221,1222,1223,1224,1225,1226,1227,
    1228,1229,1230,1231,1232,1233,1234,1235,1236,1237,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1248,1249,1251,1253,1256,1258,1260,1262,1264,1265,
    1265,1266,1267,1268,1268,1269,1270,1271,1271,1272,1273,1274,1274,1275,1276,1277,1278,1278,1279,1280,1281,1282,1283,1285,1286,1287,1288,1289,1291,1292,
    1293,1294,1295,1297,1298,1299,1300,1301,1302,1303,1304,1305,1306,1308,1309,1310,1311,1312,1313,1314,1315,1317,1318,1319,1320,1321,1323,1324,1325,1326,
    1327,1328,1329,1330,1331,1332,1332,1333,1334,1335,1336,1336,1337,1338,1339,1340,1340,1341,1342,1343,1344,1345,1347,1349,1351,1353,1355,1357,1358,1361,
    1363,1366,1368,1371,1374,1376,1377,1377,1378,1379,1379,1380,1381,1382,1382,1383,1384,1384,1385,1386,1386,1387,1388,1389,1389,1390,1391,1391,1392,1393,
    1394,1395,1396,1397,1398,1399,1399,1400,1401,1402,1403,1404,1405,1406,1407,1408,1409,1410,1411,1413,1414,1415,1416,1418,1419,1420,1421,1423,1424,1425,
    1426,1427,1427,1428,1429,1430,1431,1432,1433,1434,1435,1436,1437,1438,1439,1439,1441,1442,1443,1445,1446,1447,1449,1450,1451,1453,1454,1456,1457,1458,
    1459,1460,1461,1462,1464,1465,1466,1467,1468,1469,1470,1471,1473,1474,1475,1476,1477,1478,1479,1480,1481,1482,1483,1485,1486,1487,1488,1489,1490,1492,
    1493,1494,1496,1497,1498,1500,1501,1502,1504,1504,1505,1506,1507,1507,1508,1509,1510,1510,1511,1512,1513,1513,1514,1515,1516,1516,1517,1518,1519,1519,
    1520,1521,1522,1523,1523,1524,1525,1526,1527,1528,1528,1529,1530,1531,1532,1532,1533,1534,1535,1536,1537,1538,1539,1540,1542,1543,1544,1545,1546,1548,
    1549,1550,1551,1552,1554,1556,1558,1560,1562,1564,1566,1568,1569,1570,1571,1572,1573,1574,1575,1576,1577,1578,1579,1580,1581,1582,1583,1584,1584,1585,
    1586,1587,1587,1588,1589,1589,1590,1591,1591,1592,1593,1594,1594,1595,1596,1596,1597,1598,1599,1599,1600,1602,1605,1607,1609,1611,1613,1615,1617,1618,
    1619,1620,1621,1622,1623,1624,1625,1626,1627,1628,1629,1630,1631,1632,1634,1635,1636,1637,1638,1640,1641,1642,1643,1644,1646,1647,1648,1649,1650,1651,
    1652,1653,1654,1654,1655,1656,1657,1658,1659,1660,1661,1662,1663,1664,1665,1666,1667,1668,1670,1671,1672,1673,1674,1676,1677,1678,1679,1680,1681,1683,
    1684,1685,1686,1687,1689,1690,1691,1692,1693,1695,1696,1697,1699,1701,1703,1704,1706,1708,1709,1711,1713,1714,1715,1716,1718,1719,1720,1721,1723,1724,
    1725,1726,1727,1728,1729,1730,1731,1732,1733,1733,1734,1735,1736,1737,1737,1738,1739,1740,1741,1742,1742,1743,1744,1745,1746,1747,1748,1749,1750,1751,
    1752,1753,1754,1755,1756,1757,1759,1760,1761,1762,1763,1765,1766,1767,1769,1770,1771,1772,1774,1775,1776,1777,1777,1778,1779,1780,1780,1781,1782,1782,
    1783,1784,1785,1785,1786,1787,1788,1788,1789,1790,1790,1791,1792,1793,1794,1795,1796,1797,1798,1799,1800,1801,1802,1803,1804,1805,1806,1807,1808,1809,
    1810,1812,1813,1814,1815,1816,1818,1819,1820,1821,1822,1824,1825,1826,1828,1829,1831,1832,1834,1835,1836,1838,1839,1840,1841,1842,1843,1844,1844,1845,
    1846,1847,1847,1848,1849,1850,1851,1851,1852,1853,1854,1855,1855,1856,1857,1859,1860,1861,1862,1863,1864,1865,1866,1868,1869,1870,1871,1872,1874,1876,
    1879,1881,1883,1885,1887,1888,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1900,1901,1902,1903,1905,1906,1907,1908,1909,1910,1912,1913,1914,1915,
    1916,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1938,1939,1940,1940,1941,1942,1943,1944,1945,
    1946,1947,1948,1949,1949,1950,1951,1952,1954,1955,1956,1957,1959,1960,1961,1962,1964,1965,1966,1968,1969,1970,1971,1972,1973,1974,1975,1976,1977,1978,
    1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1990,1991,1992,1993,1994,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005,2006,2006,2007,2008,2009,
    2010,2011,2012,2013,2014,2014,2015,2016,2017,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2030,2031,2032,2038,2044,2048,2049,2050,2050,2051,2052,
    2053,2054,2054,2055,2056,2057,2057,2058,2059,2060,2061,2061,2062,2063,2064,2065,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,
    2079,2080,2081,2083,2085,2086,2088,2090,2092,2093,2095,2096,2097,2099,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2111,2112,2113,2114,2115,2117,
    2118,2119,2121,2122,2123,2125,2126,2127,2128,2129,2130,2131,2132,2133,2134,2134,2135,2136,2137,2138,2139,2140,2141,2142,2142,2143,2144,2146,2147,2148,
    2149,2150,2152,2153,2154,2155,2156,2158,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2174,2175,2176,2178,2179,2180,
    2181,2182,2183,2184,2185,2186,2187,2188,2190,2191,2192,2194,2196,2198,2200,2202,2204,2206,2208,2208,2209,2210,2210,2211,2212,2212,2213,2213,2214,2215,
    2215,2216,2216,2217,2218,2218,2219,2219,2220,2221,2221,2222,2223,2223,2224,2228,2232,2236,2240,2241,2242,2243,2244,2245,2246,2247,2248,2249,2250,2251,
    2252,2252,2253,2254,2255,2256,2257,2258,2259,2260,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2276,2277,2278,2279,2280,2282,2283,
    2284,2285,2286,2288,2289,2290,2291,2292,2293,2294,2295,2297,2298,2299,2300,2301,2302,2303,2304,2306,2307,2309,2310,2312,2313,2315,2316,2318,2319,2320,
    2321,2322,2323,2324,2324,2325,2326,2327,2328,2329,2329,2330,2331,2332,2333,2334,2334,2335,2336,2337,2338,2339,2339,2340,2341,2342,2343,2344,2345,2346,
    2346,2347,2348,2349,2350,2351,2352,2353,2354,2355,2356,2357,2358,2359,2361,2362,2363,2364,2365,2366,2367,2369,2371,2373,2374,2376,2378,2380,2381,2383,
    2384,2385,2386,2387,2388,2389,2390,2391,2392,2392,2393,2394,2395,2396,2397,2398,2399,2400,2401,2403,2405,2406,2408,2410,2412,2413,2415,2416,2417,2418,
    2419,2420,2421,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2433,2434,2435,2436,2437,2437,2438,2439,2440,2441,2442,2443,2444,2445,2446,2447,2448,
    2449,2450,2452,2453,2454,2455,2456,2457,2458,2460,2461,2462,2463,2464,2465,2466,2467,2467,2468,2469,2470,2471,2472,2473,2474,2474,2475,2476,2477,2478,
    2479,2480,2481,2483,2484,2486,2488,2489,2491,2493,2494,2496,2497,2498,2499,2500,2501,2503,2504,2505,2506,2507,2508,2509,2510,2512,2513,2514,2515,2516,
    2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2527,2528,2529,2531,2533,2534,2536,2538,2540,2541,2543,2544,2545,2545,2546,2547,2547,2548,2548,2549,
    2550,2550,2551,2551,2552,2553,2553,2554,2554,2555,2556,2556,2557,2557,2558,2558,2559,2560,2562,2566,2569,2572,2575,2577,2577,2578,2579,2580,2581,2581,
    2582,2583,2584,2585,2586,2586,2587,2588,2589,2590,2591,2591,2592,2593,2595,2596,2597,2598,2599,2600,2601,2602,2603,2605,2606,2607,2608,2609,2610,2611,
    2612,2613,2614,2616,2617,2618,2619,2620,2621,2622,2623,2624,2626,2627,2628,2629,2630,2632,2633,2634,2635,2636,2638,2639,2640,2641,2642,2644,2645,2646,
    2647,2648,2649,2650,2652,2653,2654,2655,2656,2657,2658,2658,2659,2660,2661,2661,2662,2663,2664,2664,2665,2666,2667,2667,2668,2669,2670,2670,2671,2672,
    2673,2675,2676,2677,2679,2680,2682,2683,2685,2686,2688,2689,2691,2693,2694,2696,2698,2700,2701,2703,2704,2705,2706,2706,2707,2708,2709,2709,2710,2711,
    2712,2712,2713,2714,2715,2715,2716,2717,2718,2718,2719,2720,2722,2724,2725,2727,2729,2731,2733,2734,2736,2737,2738,2739,2740,2740,2741,2742,2743,2744,
    2745,2746,2746,2747,2748,2749,2750,2751,2752,2753,2755,2756,2758,2760,2761,2763,2764,2766,2767,2769,2770,2771,2772,2773,2774,2775,2776,2777,2778,2779,
    2780,2781,2782,2783,2784,2784,2785,2786,2787,2788,2789,2790,2791,2792,2793,2794,2795,2796,2797,2798,2799,2801,2804,2807,2809,2812,2815,2817,2818,2818,
    2819,2820,2821,2822,2823,2824,2825,2826,2827,2828,2829,2830,2831,2832,2833,2834,2835,2836,2837,2838,2839,2840,2841,2842,2844,2845,2846,2847,2848,2849,
    2849,2850,2851,2852,2853,2854,2854,2855,2856,2857,2858,2859,2859,2860,2861,2862,2863,2864,2865,2865,2866,2867,2868,2869,2870,2871,2872,2872,2873,2874,
    2875,2876,2877,2878,2879,2880,2881,2882,2883,2884,2885,2886,2888,2889,2890,2891,2892,2894,2895,2896,2897,2898,2899,2900,2902,2903,2904,2905,2906,2907,
    2908,2910,2911,2912,2913,2914,2915,2916,2917,2918,2919,2920,2921,2922,2923,2924,2925,2926,2927,2928,2929,2930,2931,2932,2933,2935,2936,2937,2938,2939,
    2940,2941,2942,2943,2944,2946,2947,2948,2949,2950,2952,2953,2954,2955,2956,2958,2959,2960,2961,2962,2963,2963,2964,2965,2966,2967,2968,2969,2970,2970,
    2971,2972,2973,2974,2975,2976,2977,2979,2980,2982,2983,2985,2986,2988,2989,2991,2992,2993,2994,2995,2996,2997,2998,3000,3001,3002,3003,3004,3005,3006,
    3007,3008,3009,3010,3011,3011,3012,3013,3014,3015,3016,3017,3018,3019,3020,3021,3021,3022,3023,3024,3026,3027,3029,3030,3032,3033,3035,3036,3038,3040,
    3041,3042,3044,3045,3046,3048,3049,3050,3052,3053,3054,3056,3057,3057,3058,3059,3060,3061,3062,3063,3064,3064,3065,3066,3067,3068,3069,3070,3071,3072,
    3073,3074,3076,3077,3079,3080,3082,3083,3085,3086,3088,3089,3089,3090,3091,3092,3093,3094,3094,3095,3096,3097,3098,3099,3100,3100,3101,3102,3103,3104,
    3105,3106,3107,3108,3109,3111,3112,3113,3114,3115,3116,3118,3119,3120,3120,3121,3122,3123,3123,3124,3125,3125,3126,3127,3127,3128,3129,3129,3130,3131,
    3131,3132,3133,3133,3134,3135,3135,3137,3140,3142,3144,3147,3149,3151,3153,3154,3155,3156,3157,3158,3159,3160,3161,3162,3163,3164,3166,3167,3168,3169,
    3169,3170,3171,3172,3173,3174,3175,3175,3176,3177,3178,3179,3180,3181,3181,3182,3183,3184,3185,3187,3188,3189,3190,3191,3193,3194,3195,3196,3197,3198,
    3200,3201,3202,3203,3204,3205,3206,3207,3208,3209,3210,3211,3212,3213,3214,3215,3216,3217,3218,3219,3220,3221,3222,3223,3224,3225,3226,3228,3229,3230,
    3231,3232,3233,3234,3235,3236,3237,3238,3239,3240,3241,3242,3243,3244,3245,3246,3247,3248,3249,3250,3250,3251,3252,3253,3253,3254,3255,3256,3256,3257,
    3258,3259,3259,3260,3261,3261,3262,3263,3264,3265,3267,3268,3269,3271,3272,3274,3275,3276,3278,3279,3280,3281,3282,3283,3284,3285,3286,3286,3287,3288,
    3289,3290,3291,3292,3293,3293,3294,3295,3296,3297,3298,3299,3300,3301,3302,3303,3304,3305,3306,3307,3308,3309,3309,3310,3311,3313,3315,3316,3318,3320,
    3322,3324,3325,3327,3328,3329,3330,3330,3331,3332,3333,3333,3334,3335,3335,3336,3337,3337,3338,3339,3340,3340,3341,3342,3342,3343,3344,3345,3346,3347,
    3348,3349,3350,3351,3352,3353,3354,3355,3356,3357,3358,3359,3360,3361,3362,3363,3364,3365,3366,3367,3368,3369,3370,3371,3372,3373,3374,3375,3376,3377,
    3378,3379,3380,3381,3382,3383,3384,3385,3386,3387,3388,3389,3390,3391,3392,3393,3394,3394,3395,3396,3397,3398,3399,3400,3401,3402,3403,3404,3405,3406,
    3406,3407,3408,3409,3409,3410,3411,3411,3412,3413,3413,3414,3415,3415,3416,3416,3417,3418,3418,3419,3420,3420,3421,3422,3422,3423,3423,3426,3430,3435,
    3440,3440,3441,3441,3441,3442,3442,3443,3443,3444,3444,3444,3445,3445,3446,3446,3446,3447,3447,3448,3448,3449,3449,3449,3450,3450,3451,3451,3452,3452,
    3452,3453,3453,3454,3454,3454,3455,3455,3456,3457,3459,3461,3463,3465,3466,3468,3470,3472,3473,3474,3475,3476,3477,3478,3479,3480,3481,3482,3483,3484,
    3485,3486,3487,3488,3488,3489,3490,3490,3491,3492,3493,3493,3494,3495,3495,3496,3497,3497,3498,3499,3499,3500,3501,3501,3502,3503,3503,3504,3505,3506,
    3506,3507,3508,3509,3509,3510,3511,3512,3512,3513,3514,3515,3515,3516,3517,3518,3518,3519,3520,3521,3521,3522,3523,3524,3525,3525,3526,3527,3528,3529,
    3530,3530,3531,3532,3533,3534,3534,3535,3536,3536,3537,3537,3538,3538,3539,3539,3539,3540,3540,3541,3541,3542,3542,3543,3543,3543,3544,3544,3545,3545,
    3546,3546,3547,3547,3547,3548,3548,3549,3549,3550,3550,3551,3551,3551,3553,3556,3560,3564,3568,3568,3569,3570,3570,3571,3571,3572,3573,3573,3574,3574,
    3575,3576,3576,3577,3577,3578,3578,3579,3580,3580,3581,3581,3582,3583,3583,3584,3585,3585,3586,3587,3588,3588,3589,3590,3591,3591,3592,3593,3594,3594,
    3595,3596,3597,3597,3598,3599,3600,3600,3601,3602,3603,3603,3604,3605,3605,3606,3607,3607,3608,3609,3610,3610,3611,3612,3612,3613,3614,3615,3615,3616,
    3617,3618,3618,3619,3620,3621,3621,3622,3623,3624,3625,3625,3626,3627,3628,3628,3629,3630,3631,3632,3632,3633,3634,3635,3635,3636,3637,3638,3638,3639,
    3640,3641,3641,3642,3643,3644,3644,3645,3646,3647,3647,3648,3649,3649,3650,3651,3651,3652,3652,3653,3654,3654,3655,3655,3656,3657,3657,3658,3658,3659,
    3660,3660,3661,3661,3662,3663,3663,3664,3665,3666,3667,3668,3669,3670,3671,3673,3674,3675,3676,3677,3678,3679,3680,3681,3682,3683,3684,3684,3685,3686,
    3687,3688,3689,3690,3690,3691,3692,3693,3694,3695,3696,3696,3697,3697,3698,3699,3699,3700,3700,3701,3702,3702,3703,3703,3704,3705,3705,3706,3706,3707,
    3707,3708,3709,3709,3710,3710,3711,3712,3712,3713,3714,3714,3715,3716,3716,3717,3718,3718,3719,3719,3720,3721,3721,3722,3723,3723,3724,3725,3725,3726,
    3727,3727,3728,3729,3729,3730,3731,3731,3732,3733,3733,3734,3735,3735,3736,3737,3737,3738,3739,3739,3740,3741,3741,3742,3743,3743,3744,3745,3746,3746,
    3747,3748,3749,3750,3751,3751,3752,3753,3754,3755,3756,3756,3757,3758,3759,3760,3760,3761,3762,3762,3763,3764,3764,3765,3766,3766,3767,3768,3768,3769,
    3770,3770,3771,3772,3772,3773,3774,3774,3775,3776,3776,3777,3778,3778,3779,3779,3780,3781,3781,3782,3783,3783,3784,3785,3785,3786,3787,3787,3788,3789,
    3789,3790,3790,3791,3792,3792,3793,3794,3794,3795,3795,3796,3796,3797,3798,3798,3799,3799,3800,3801,3801,3802,3802,3803,3803,3804,3805,3805,3806,3806,
    3807,3808,3808,3809,3809,3810,3810,3811,3811,3812,3812,3813,3813,3814,3814,3815,3815,3816,3816,3817,3817,3818,3818,3819,3819,3820,3820,3821,3822,3822,
    3823,3823,3824,3824,3825,3826,3827,3828,3829,3830,3831,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3841,3842,3842,3843,3843,3844,3844,3845,3845,
    3846,3846,3847,3847,3848,3848,3849,3849,3850,3850,3851,3851,3852,3852,3853,3853,3854,3854,3855,3855,3856,3856,3857,3858,3858,3859,3860,3861,3861,3862,
    3863,3863,3864,3865,3865,3866,3867,3867,3868,3869,3870,3870,3871,3872,3872,3873,3873,3874,3874,3875,3875,3876,3877,3877,3878,3878,3879,3879,3880,3880,
    3881,3881,3882,3882,3883,3883,3884,3884,3885,3886,3886,3887,3887,3888,3888,3889,3890,3890,3891,3892,3892,3893,3894,3894,3895,3896,3896,3897,3898,3898,
    3899,3900,3900,3901,3902,3902,3903,3904,3904,3905,3906,3906,3907,3908,3908,3909,3909,3910,3911,3911,3912,3913,3913,3914,3914,3915,3916,3916,3917,3918,
    3918,3919,3919,3920,3921,3921,3922,3923,3923,3924,3925,3925,3926,3926,3927,3928,3928,3929,3930,3930,3931,3932,3932,3933,3933,3934,3935,3935,3936,3936,
    3937,3937,3938,3938,3939,3939,3940,3941,3941,3942,3942,3943,3943,3944,3944,3945,3945,3946,3946,3947,3947,3948,3948,3949,3949,3950,3950,3951,3951,3952,
    3952,3953,3954,3954,3955,3955,3956,3957,3957,3958,3959,3959,3960,3960,3961,3962,3962,3963,3963,3964,3965,3965,3966,3967,3967,3968,3969,3969,3970,3971,
    3971,3972,3973,3973,3974,3975,3975,3976,3977,3977,3978,3979,3979,3980,3981,3981,3982,3983,3983,3984,3984,3985,3985,3985,3986,3986,3987,3987,3987,3988,
    3988,3989,3989,3989,3990,3990,3991,3991,3991,3992,3992,3992,3993,3993,3994,3994,3994,3995,3995,3996,3996,3996,3997,3997,3997,3998,3998,3999,3999,3999,
    4000,4001,4002,4004,4005,4006,4007,4009,4010,4011,4013,4014,4015,4016,4016,4017,4017,4018,4018,4019,4019,4019,4020,4020,4021,4021,4022,4022,4022,4023,
    4023,4024,4024,4025,4025,4025,4026,4026,4027,4027,4028,4028,4028,4029,4029,4030,4030,4031,4031,4032,4032,4033,4033,4034,4034,4035,4036,4036,4037,4037,
    4038,4039,4039,4040,4040,4041,4042,4042,4043,4044,4044,4045,4045,4046,4047,4047,4048,4048,4049,4049,4050,4051,4051,4052,4052,4053,4053,4054,4054,4055,
    4055,4056,4057,4057,4058,4058,4059,4059,4060,4060,4061,4061,4062,4063,4063,4064,4064,4065,4066,4066,4067,4068,4068,4069,4070,4070,4071,4072,4073,4073,
    4074,4075,4075,4076,4077,4077,4078,4079,4079,4080,4080,4081,4081,4082,4082,4083,4083,4084,4084,4084,4085,4085,4086,4086,4087,4087,4088,4088,4088,4089,
    4089,4090,4090,4091,4091,4092,4092,4092,4093,4093,4094,4094,4095,4095,4095};

class EnergyMonitor
{
  public:

    void voltage(unsigned int _inPinV, double _VCAL, double _PHASECAL);
    void current(unsigned int _inPinI, double _ICAL);

    void voltageTX(double _VCAL, double _PHASECAL);
    void currentTX(unsigned int _channel, double _ICAL);

    void calcVI(unsigned int crossings, unsigned int timeout);
    double calcIrms(unsigned int NUMBER_OF_SAMPLES);
    void serialprint();

    long readVcc();
    //Useful value variables
    double realPower,
      apparentPower,
      powerFactor,
      Vrms,
      Irms;

  private:

    //Set Voltage and current input pins
    unsigned int inPinV;
    unsigned int inPinI;
    //Calibration coefficients
    //These need to be set in order to obtain accurate results
    double VCAL;
    double ICAL;
    double PHASECAL;

    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
    int sampleV;                        //sample_ holds the raw analog read value
    int sampleI;

    double lastFilteredV,filteredV;          //Filtered_ is the raw analog value minus the DC offset
    double filteredI;
    double offsetV;                          //Low-pass filter output
    double offsetI;                          //Low-pass filter output

    double phaseShiftedV;                             //Holds the calibrated phase shifted voltage.

    double sqV,sumV,sqI,sumI,instP,sumP;              //sq = squared, sum = Sum, inst = instantaneous

    int startV;                                       //Instantaneous voltage at start of sample window.

    boolean lastVCross, checkVCross;                  //Used to measure number of times threshold is crossed.


};

#endif