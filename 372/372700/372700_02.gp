M=10000;

a(n) = sum(k=1, n, moebius(n*k)); 
my(s=-1); for(n=1, M, s+=2*a(n); write("/Users/xxx/Desktop/b372700_1.txt", n, " ", s))

