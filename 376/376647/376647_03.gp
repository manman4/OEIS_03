\\ a(n) = a(n-6) + a(n-7).
a(n) = my(v=[1, 0, 0, 1, 0, 0, 1]); if(n<7, v[n+1], a(n-6) + a(n-7));
for(n=0, 55, print1(a(n),", "))


