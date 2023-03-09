M=499;

a(n) = fromdigits(Vec(Pol(digits(n))^n)%10);
for(n=0, M, write("/Users/xxx/Desktop/b361351_01.txt", n, " ", a(n)))