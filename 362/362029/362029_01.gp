a(n) = sum(k=1, n, (-1)^(n-k)*k*moebius(k)^2);

for(n=1, 10000, write("/Users/xxx/Desktop/b362029_1.txt", n, " ", a(n)))

