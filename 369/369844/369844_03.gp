\\ offset = 1
a(n) = my(v=[0,1,4,11,29]); if(n<6,v[n], 5*a(n-1) - 9*a(n-2) + 10*a(n-3) - 5*a(n-4) + a(n-5) );
for(n=1, 20, print1(a(n),", "))
