a(n) = my(v=[1,-1,5,1,19,30]); if(n<6, v[n+1], -a(n-1) + 4*a(n-2) + 10*a(n-3) + 10*a(n-4) + 5*a(n-5) + a(n-6) );                                                                     
for(n=0, 25, print1(a(n),", ")) 

   