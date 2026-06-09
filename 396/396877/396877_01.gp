\\ a(0) = 4. For n > 0, let k = ceiling((sqrt(n+1)-1)/2), m = (n - 4*k*(k-1) - 1) mod 4*k. a(n) = 2*k - 1 + (m mod 2) + 2*[m >= 2*k and m even].
a(n) = if(n==0, 4, my(k=ceil((sqrt(n+1)-1)/2), m=(n-4*k*(k-1)-1)%(4*k)); 2*k-1+m%2+2*(m>=2*k)*(1-m%2));
M=10000;
for(n=0, M, write("b396877_01.txt", n, " ", a(n)));
