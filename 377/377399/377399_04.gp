\\ a(n) = 9*a(n-1) - 26*a(n-2) + 24*a(n-3) + 192 for n > 3.
a(n) = my(v=[1,-4,8,8]); if(n<4, v[n+1], 9*a(n-1) - 26*a(n-2) + 24*a(n-3) + 192);
for(n=0, 20, print1(a(n), ", "))

\\ 以下は間違い
a(n) = my(v=[1,-4,8,8]); if(n<3, v[n+1], 9*a(n-1) - 26*a(n-2) + 24*a(n-3) + 192);
for(n=0, 20, print1(a(n), ", "))

