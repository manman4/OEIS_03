a(n) =my(v=[1, 1, 1, 1, 1, 241, 1441, 5041, 13441]); if(n<9, v[n+1], a(n-1) + 10*(n-1)*(n-2)*(n-3)*(n-4)*a(n-5) + 9*(n-1)*(n-2)*(n-3)*(n-4)*(n-5)*(n-6)*(n-7)*(n-8)*a(n-9) );   
for(n=0, 22, print1(a(n),", "))

