\\ n<5ではだめ
a(n) = my(v=[1,0,1,5,16,45,126]); if(n<6, v[n+1], 5*a(n-1) - 9*a(n-2) + 10*a(n-3) - 5*a(n-4) + a(n-5) );
for(n=0, 25, print1(a(n), ", "));