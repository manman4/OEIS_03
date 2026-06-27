\\ For m >= 0 and any constants r, s, define a_{m,r,s}(n) = Sum_{k=0..n} binomial(n+(m-1)*k,m*k) * (r*binomial(n+m*k,k) - s*binomial(n+m*k,k-1)). 
\\ A_{m,r,s}(x) = Sum_{n>=0} a_{m,r,s}(n)*x^n = t*(1+t^m)*(r+(r+s)*t^(m-1)-s*t^m)/((1+t^(m-1))*(1+(m+1)*t^m-m*t^(m+1))), where t = t(x) satisfies t = 1 + x*t*(1+t^m).


\\ Proof: Let
\\ a_{m,r,s}(n) = Sum_{k=0..n} binomial(n+(m-1)*k, m*k) * (r*binomial(n+m*k, k) - s*binomial(n+m*k, k-1))
\\ and let A_{m,r,s}(x) = Sum_{n>=0} a_{m,r,s}(n)*x^n.
\\ Set
\\ R_{m,r,s}(t) = t*(1+t^m)*(r+(r+s)*t^(m-1)-s*t^m)/((1+t^(m-1))*(1+(m+1)*t^m-m*t^(m+1))),
\\ where t = t(x) is defined by
\\ t = 1 + x*t*(1+t^m).
\\ We show that A_{m,r,s}(x) = R_{m,r,s}(t(x)).
\\ Since t has constant term 1, the powers t^(m-1) and t^m are well-defined formal power series also when m=0.
\\ Put y = t-1. Then y = x*t*(1+t^m), so
\\ x = (t-1)/(t*(1+t^m)) = y/(t*(1+t^m)),
\\ and
\\ dx/dt = (1+(m+1)*t^m-m*t^(m+1))/(t^2*(1+t^m)^2).
\\ By residue/Lagrange inversion,
\\ [x^n] A_{m,r,s}(x)
\\ = [x^n] R_{m,r,s}(t(x))
\\ = Res R_{m,r,s}(t) * dx/x^(n+1)
\\ = [y^n] R_{m,r,s}(t) * (dx/dt) * (t*(1+t^m))^(n+1).
\\ Substituting R_{m,r,s}(t) and dx/dt, the factor 1+(m+1)*t^m-m*t^(m+1) cancels, and we get
\\ [x^n] A_{m,r,s}(x)
\\ = [y^n] t^n*(1+t^m)^n*(r+(r+s)*t^(m-1)-s*t^m)/(1+t^(m-1)).     (1)
\\ Now write
\\ B = 1+t^(m-1),
\\ q = y*t^(m-1).
\\ Since t = 1+y, we have
\\ 1+t^m = 1+t^(m-1)+y*t^(m-1) = B+q,
\\ and
\\ r+(r+s)*t^(m-1)-s*t^m = r*B-s*q.
\\ Hence (1) becomes
\\ [x^n] A_{m,r,s}(x)
\\ = [y^n] t^n*(B+q)^n*(r*B-s*q)/B.
\\ Expand:
\\ (B+q)^n*(r*B-s*q)/B
\\ = Sum_{k=0..n} (r*binomial(n,k) - s*binomial(n,k-1)) * q^k * B^(n-k).
\\ Therefore
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{k=0..n} (r*binomial(n,k) - s*binomial(n,k-1))
\\   * [y^(n-k)] t^(n+(m-1)*k) * (1+t^(m-1))^(n-k).
\\ Expand again:
\\ (1+t^(m-1))^(n-k) = Sum_{j=0..n-k} binomial(n-k,j) * t^((m-1)*j).
\\ Thus
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{k=0..n} Sum_{j=0..n-k}
\\   (r*binomial(n,k) - s*binomial(n,k-1))
\\   * binomial(n-k,j)
\\   * [y^(n-k)] t^(n+(m-1)*(k+j)).
\\ Since t = 1+y,
\\ [y^(n-k)] t^(n+(m-1)*(k+j))
\\ = binomial(n+(m-1)*(k+j), n-k).
\\ Let h = k+j. Then 0 <= h <= n and 0 <= k <= h, so
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{h=0..n} Sum_{k=0..h}
\\   (r*binomial(n,k) - s*binomial(n,k-1))
\\   * binomial(n-k, h-k)
\\   * binomial(n+(m-1)*h, n-k).     (2)
\\ Now use
\\ binomial(n-k, h-k) * binomial(n+(m-1)*h, n-k)
\\ = binomial(n+(m-1)*h, n-h) * binomial(m*h, h-k).
\\ So (2) becomes
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{h=0..n} binomial(n+(m-1)*h, n-h)
\\   * Sum_{k=0..h} (r*binomial(n,k) - s*binomial(n,k-1)) * binomial(m*h, h-k).
\\ By Vandermonde's convolution,
\\ Sum_{k=0..h} binomial(n,k) * binomial(m*h, h-k)
\\ = binomial(n+m*h, h),
\\ and similarly
\\ Sum_{k=0..h} binomial(n,k-1) * binomial(m*h, h-k)
\\ = binomial(n+m*h, h-1).
\\ Hence
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{h=0..n} binomial(n+(m-1)*h, n-h)
\\   * (r*binomial(n+m*h, h) - s*binomial(n+m*h, h-1)).
\\ Finally,
\\ binomial(n+(m-1)*h, n-h) = binomial(n+(m-1)*h, m*h),
\\ so
\\ [x^n] A_{m,r,s}(x)
\\ = Sum_{h=0..n} binomial(n+(m-1)*h, m*h)
\\   * (r*binomial(n+m*h, h) - s*binomial(n+m*h, h-1))
\\ = a_{m,r,s}(n).
\\ Therefore
\\ A_{m,r,s}(x) = t*(1+t^m)*(r+(r+s)*t^(m-1)-s*t^m)/((1+t^(m-1))*(1+(m+1)*t^m-m*t^(m+1))),
\\ where t satisfies
\\ t = 1 + x*t*(1+t^m).
\\ This proves the claimed general formula.

default(parisize, 200000000);
N = 22;

m = 4;
r = 1;
s = -1;

a(n) = sum(k=0, n, binomial(n + (m-1)*k, m*k) * (r*binomial(n + m*k, k) - s*binomial(n + m*k, k-1)));

t = 1 + x + O(x^(N+1));
for(i=1, N+3, t = 1 + x*t*(1 + t^m) + O(x^(N+1)));
A = t*(1 + t^m)*(r + (r+s)*t^(m-1) - s*t^m) / ((1 + t^(m-1))*(1 + (m+1)*t^m - m*t^(m+1)));
print("m = ", m, ", r = ", r, ", s = ", s);
print("t(x) = ", t);
print("A(x) = ", A + O(x^(N+1)));
print("Check sum formula against A(x):");
for(n=0, N, print([n, polcoeff(A,n), a(n), if(polcoeff(A,n)==a(n), "ok", "fail")]));

print("Coefficients of A(x):");
print(Vec(A));
