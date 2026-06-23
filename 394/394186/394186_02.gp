\\ Proof: Let
\\ a_m(n) = Sum_{k=0..n} binomial(n+(m-1)k, mk) * (binomial(n+mk, k) - binomial(n+mk, k-1))
\\ and let A_m(x) = Sum_{n>=0} a_m(n)*x^n.
\\ Set
\\ R_m(t) = t*(1+t^m)(1+2t^(m-1)-t^m)/((1+t^(m-1))*(1+(m+1)t^m-mt^(m+1))),
\\ where t = t(x) is defined by
\\ t = 1 + xt(1+t^m).
\\ We show that A_m(x) = R_m(t(x)).
\\ Put y = t-1. Then y = xt(1+t^m), so
\\ x = (t-1)/(t*(1+t^m)) = y/(t*(1+t^m)),
\\ and
\\ dx/dt = (1+(m+1)t^m-mt^(m+1))/(t^2*(1+t^m)^2).
\\ By residue/Lagrange inversion,
\\ [x^n] A_m(x)
\\ = [x^n] R_m(t(x))
\\ = Res R_m(t) dx/x^(n+1)
\\ = [y^n] R_m(t) * (dx/dt) * (t*(1+t^m))^(n+1).
\\ Substituting R_m(t) and dx/dt, the factor 1+(m+1)t^m-mt^(m+1) cancels, and we get
\\ [x^n] A_m(x)
\\ = [y^n] t^n*(1+t^m)^n*(1+2*t^(m-1)-t^m)/(1+t^(m-1)).     (1)
\\ Now write
\\ B = 1+t^(m-1),
\\ q = y*t^(m-1).
\\ Since t = 1+y, we have
\\ 1+t^m = 1+t^(m-1)+y*t^(m-1) = B+q,
\\ and
\\ 1+2*t^(m-1)-t^m = B-q.
\\ Hence (1) becomes
\\ [x^n] A_m(x)
\\ = [y^n] t^n*(B+q)^n*(B-q)/B.
\\ Expand:
\\ (B+q)^n*(B-q)/B
\\ = Sum_{k=0..n} (binomial(n,k) - binomial(n,k-1)) * q^k * B^(n-k).
\\ Therefore
\\ [x^n] A_m(x)
\\ = Sum_{k=0..n} (binomial(n,k) - binomial(n,k-1))
\\[y^(n-k)] t^(n+(m-1)*k) * (1+t^(m-1))^(n-k).
\\ Expand again:
\\ (1+t^(m-1))^(n-k) = Sum_{j=0..n-k} binomial(n-k,j) * t^((m-1)*j).
\\ Thus
\\ [x^n] A_m(x)
\\ = Sum_{k=0..n} Sum_{j=0..n-k}
\\   (binomial(n,k) - binomial(n,k-1))
\\ binomial(n-k,j)
\\ [y^(n-k)] t^(n+(m-1)(k+j)).
\\ Since t = 1+y,
\\ [y^(n-k)] t^(n+(m-1)(k+j))
\\ = binomial(n+(m-1)(k+j), n-k).
\\ Let r = k+j. Then 0 <= r <= n and 0 <= k <= r, so
\\ [x^n] A_m(x)
\\ = Sum_{r=0..n} Sum_{k=0..r}
\\   (binomial(n,k) - binomial(n,k-1))
\\ binomial(n-k, r-k)
\\ binomial(n+(m-1)r, n-k).     (2)
\\ Now use
\\ binomial(n-k, r-k) * binomial(n+(m-1)r, n-k)
\\ = binomial(n+(m-1)r, n-r) * binomial(m*r, r-k).
\\ So (2) becomes
\\ [x^n] A_m(x)
\\ = Sum_{r=0..n} binomial(n+(m-1)r, n-r)
\\ Sum_{k=0..r} (binomial(n,k) - binomial(n,k-1)) * binomial(m*r, r-k).
\\ By Vandermonde's convolution,
\\ Sum_{k=0..r} binomial(n,k) * binomial(mr, r-k)
\\ = binomial(n+mr, r),
\\ and similarly
\\ Sum_{k=0..r} binomial(n,k-1) * binomial(mr, r-k)
\\ = binomial(n+mr, r-1).
\\ Hence
\\ [x^n] A_m(x)
\\ = Sum_{r=0..n} binomial(n+(m-1)r, n-r)
\\ (binomial(n+mr, r) - binomial(n+mr, r-1)).
\\ Finally,
\\ binomial(n+(m-1)r, n-r) = binomial(n+(m-1)r, m*r),
\\ so
\\ [x^n] A_m(x)
\\ = Sum_{r=0..n} binomial(n+(m-1)r, m*r)
\\ (binomial(n+mr, r) - binomial(n+mr, r-1))
\\ = a_m(n).
\\ Therefore
\\ A_m(x) = t*(1+t^m)(1+2t^(m-1)-t^m)/((1+t^(m-1))*(1+(m+1)t^m-mt^(m+1))),
\\ where t satisfies
\\ t = 1 + xt(1+t^m).
\\ This proves the claimed general formula.




default(parisize, 200000000);
N = 22;

m = 4;
r = 0;
s = -1;

a(n) = sum(k=0, n, binomial(n + (m-1)*k, m*k) * (r*binomial(n + m*k, k) - s*binomial(n + m*k, k-1)));

t = 1 + x + O(x^(N+1));
for(i=1, N+3, t = 1 + x*t*(1 + t^m) + O(x^(N+1)));

A = t*(1 + t^m)*(r + (r+s)*t^(m-1) - s*t^m) / ((1 + t^(m-1))*(1 + (m+1)*t^m - m*t^(m+1)));

print("m = ", m, ", r = ", r, ", s = ", s);
print("t(x) = ", t);
print("A(x) = ", A + O(x^(N+1)));

print("Check sum formula against A(x):");
for(n=0, N, print([n, polcoeff(A,n), a(n), polcoeff(A,n)==a(n)]));

print("Coefficients of A(x):");
print(Vec(A));