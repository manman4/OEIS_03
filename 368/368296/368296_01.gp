\\ T(n,k) = Sum_{j=0..n} k^(n-j) * floor(j/2).
T(n, k) = sum(j=0, n, k^(n-j)*(j\2));

S(n, k) = if(n<2, 0, S(n-2, k) + sum(j=0, n-2, k^j));

U(n, k) = 1/(k+1) * (-((n+1)\2)+sum(j=1, n, j*k^(n-j))); 

\\ 失敗例
V(n, k) = 1/(k-1) * sum(j=0, n, k^j\(k+1));
for(n=0, 20, print1(V(n, 0), ", "));
\\ 正しい方
V(n, k) = if(k<2, T(n,k), 1/(k-1) * sum(j=0, n, k^j\(k+1)));
\\ 失敗例
V2(n, k) = sum(j=0, n, k^j\(k^2-1));
for(n=0, 20, print1(V2(n, 0), ", "));
V2(n, k) = if(k<2, T(n,k), sum(j=0, n, k^j\(k^2-1)));
\\ V(n, k) = V2(n, k)の確認
for(k=2, 100, for(n=0, 200, if(V(n, k)!=V2(n, k), print1([k, n], ", "))));

W(n, k) = if(n<4, T(n,k), (k+1)*W(n-1,k) - (k-1)*W(n-2,k) - (k+1)*W(n-3,k) + k*W(n-4,k));

X(n, k) = polcoeff(x^2/((1-x) * (1-k*x) * (1-x^2) + x*O(x^n)), n);

\\ 失敗例
Y(n, k) = 1/(k-1) * (k^(n+1)\(k^2-1) - (n+1)\2);
for(n=0, 20, print1(Y(n, 0), ", "));
Y(n, k) = if(k<2, T(n,k), 1/(k-1) * (k^(n+1)\(k^2-1) - (n+1)\2));

\\ 一般の場合
a(n, m, k) = (k^(n+1)\(k^m-1)-(n+1)\m)/(k-1);    
b(n, m, k) = sum(j=0, n, k^(n-j)*(j\m));
for(k=2, 20, for(m=2, 20, for(n=0, 100, if(a(n, m, k)!=b(n, m, k), print1([k, m, n], ", ")))));

for(n=2, 12, for(k=2, n, print1(T(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-S(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-U(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-V(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-V2(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-W(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-X(k,n-k), ", ")));
for(n=2, 13, for(k=2, n, print1(T(k,n-k)-Y(k,n-k), ", ")));

