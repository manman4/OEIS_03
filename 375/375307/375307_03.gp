a(n) = my(v=[1,1,4,16,52]); if(n<5, v[n+1], 2*a(n-1) + 5*a(n-2) + 3*a(n-3) - 3*a(n-4) + a(n-5) );                                                                     
for(n=0, 26, print1(a(n),", ")) 

   