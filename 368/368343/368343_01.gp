\\ T(n,k) = Sum_{j=0..n} k^(n-j) * floor(j/3).
T(n, k) = sum(j=0, n, k^(n-j)*(j\3));

S(n, k) = if(n<3, 0, S(n-3, k) + sum(j=0, n-3, k^j));

\\ U(n, k) = 1/(k+1) * (-((n+1)\3)+sum(j=1, n, j*k^(n-j))); 

\\ 失敗例
V(n, k) = 1/(k-1) * sum(j=0, n, k^j\(k^2+k+1));
for(n=0, 20, print1(V(n, 0), ", "));
\\ 正しい方
V(n, k) = if(k<2, T(n,k), 1/(k-1) * sum(j=0, n, k^j\(k^2+k+1)));
\\ 失敗例
V2(n, k) = sum(j=0, n, k^j\(k^3-1));
for(n=0, 20, print1(V2(n, 0), ", "));
V2(n, k) = if(k<2, T(n,k), sum(j=0, n, k^j\(k^3-1)));
\\ V(n, k) = V2(n, k)の確認
for(k=2, 100, for(n=0, 100, if(V(n, k)!=V2(n, k), print1([k, n], ", "))));
print;

W(n, k) = if(n<5, T(n,k), (k+1)*W(n-1,k) - k*W(n-2,k) + W(n-3,k) - (k+1)*W(n-4,k) + k*W(n-5,k));

X(n, k) = polcoeff(x^3/((1-x) * (1-k*x) * (1-x^3) + x*O(x^n)), n);

Y(n, k) = if(k<2, T(n,k), 1/(k-1) * (k^(n+1)\(k^3-1) - (n+1)\3));


for(n=3, 13, for(k=3, n,  print1(T(k,n-k), ", ")));
for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-S(k,n-k), ", ")));
\\ for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-U(k,n-k), ", ")));
for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-V(k,n-k), ", ")));
for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-V2(k,n-k), ", ")));
for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-W(k,n-k), ", ")));
for(n=3, 13, for(k=3, n,  print1(T(k,n-k)-X(k,n-k), ", ")));
for(n=3, 13, for(k=3, n, print1(T(k,n-k)-Y(k,n-k), ", ")));
