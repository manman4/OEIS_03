\\ n<7ではだめ
a(n) = my(v=[1,0,0,0,0,0,1,7]); if(n<8, v[n+1], 7*a(n-1) - 21*a(n-2) + 35*a(n-3) - 35*a(n-4) + 21*a(n-5) - 6*a(n-6) + a(n-7) );
for(n=0, 25, print1(a(n), ", "));