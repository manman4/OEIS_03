a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 4^(i-j)*abs(stirling(i+1, j, 1))*v[j])); v;
a_vector(30)

\\ E.g.f. A(x) satisfies A(-log(1-4*x)/4) = 5*A(x) - 4*x.
N=30;
v=a_vector(N);
X=x+O(x^(N+1));
A=sum(n=1,N,v[n]*X^n/n!);
P=-log(1-4*X)/4;
if(subst(A,x,P)==5*A-4*X, print("ok"), print("not ok"));


\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 4^(n-k) * |Stirling1(n+1,k)| * a(k).
\\ a(1) = 1; a(n) = Sum_{k=1..n-1} 4^(n-1-k) * |Stirling1(n,k)| * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 4^(n-1-k) * abs(stirling(n, k, 1)) * a(k)));
for(n=1, 15, print1(a(n),", "));

