M=22;

a(n)= my(v=[1,1,2,7,26,126]); if(n<6, v[n+1], (n-1) * a(n-1) + (n-2) * a(n-2) + (n-4) * a(n-3) - 2 * a(n-4) - 2 * a(n-5) + 3);
for(n=0, M, print1(a(n), ", "));