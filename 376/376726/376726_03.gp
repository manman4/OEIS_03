\\ a(n) = 2*a(n-2) + 2*a(n-3) - a(n-4) + 2*a(n-5) - a(n-6).
a(n) = my(v=[1, 0, 3, 1, 5, 10]); if(n<6, v[n+1], 2*a(n-2) + 2*a(n-3) - a(n-4) + 2*a(n-5) - a(n-6));
for(n=0, 25, print1(a(n),", "))

