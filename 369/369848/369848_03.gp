\\ offset = 1
a(n) = my(v=[0,1,3,6,11]); if(n<6,v[n], 5*a(n-1) - 10*a(n-2) + 11*a(n-3) - 5*a(n-4) + a(n-5) );
for(n=1, 20, print1(a(n),", "))
