default(realprecision, 120);

M=1000;

a(n) = floor(abs(tan(n)));
for(n=0, M, write("/Users/xxx/Desktop/b328376.txt", n, " ", a(n)))