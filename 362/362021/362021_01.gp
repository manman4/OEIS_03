a(n) = sum(k=1, n, (-1)^(n-k)*k*moebius(k));

for(n=1, 10000, write("/Users/xxx/Desktop/b362021_1.txt", n, " ", a(n)))
