//This file was generated from (Academic) UPPAAL 4.0.9 (rev. 4407), September 2009

/*

*/
A[] (not deadlock)

/*
[Heart condition 1] Bradycardia: the v-v interval should be no bigger than TLRI no matter how we set the heart rate for random heart. We can\u2019t say the same thing for a-a interval.
*/
A[] (PLRL.two_v imply PLRL.t<=TLRI)

/*
[Heart condition 1] For URI component: A ventricle pace(VP) can only happen at least TURI after a ventricle event(VS VP)
*/
A[] (PURL.interval imply PURL.t>=TURI)

/*

*/
A[] (not PPersist.err)
