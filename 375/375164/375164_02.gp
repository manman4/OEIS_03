a(n) = my(v=[1, 4, 10, 20]); if(n<4, v[n+1], 4*a(n-1) - 6*a(n-2) + 4*a(n-3) + a(n-4));
for(n=0, 25, print1(a(n),", "))   
