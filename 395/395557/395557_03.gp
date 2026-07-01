\\ Here and below, binomial(N,k)=0 for k<0.
\\ For integers l, m, c in Z, and for any constants r and s, define
\\ a_{l,m,c,r,s}(n) = Sum_{k=0..n} r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1).
\\ A_{l,m,c,r,s}(x) = Sum_{n>=0} a_{l,m,c,r,s}(n)*x^n = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m)),
\\ where t = t(x) satisfies t = 1 + x*t^(l+m), equivalently y = x*(1+y)^(l+m) with y=t-1.


\\ Proof: Here and below, binomial(n,k)=0 for k<0.
\\
\\ For integers l, m, c in Z, and any constants r, s, define
\\ a_{l,m,c,r,s}(n)
\\ = Sum_{k=0..n} (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)),
\\ and let
\\ A_{l,m,c,r,s}(x) = Sum_{n>=0} a_{l,m,c,r,s}(n)*x^n.
\\ We prove that
\\ A_{l,m,c,r,s}(x)
\\ = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m)),
\\ where t = t(x) is the unique formal power series with constant term 1 satisfying
\\ t = 1 + x*t^(l+m).
\\
\\ Put d = l+m and y = t-1. Then t = 1+y and
\\ y = x*t^d = x*(1+y)^d.
\\ Since (1+y)^d is a well-defined formal power series with constant term 1 for every integer d,
\\ this determines a unique formal power series y = y(x), hence a unique t = t(x).
\\ Also, since t has constant term 1, the series t^c and t^m are well-defined for every integer c,m,
\\ and both d-(d-1)*t and 1-t+t^m have constant term 1, so R(t) below is a well-defined formal power series in y.
\\ Also,
\\ x = (t-1)/t^d,
\\ so
\\ dx/dt = (d-(d-1)*t)/t^(d+1).
\\
\\ Set
\\ R(t) = t^c*(r+s-s*t)/((d-(d-1)*t)*(1-t+t^m)).
\\ We show that A_{l,m,c,r,s}(x) = R(t(x)).
\\
\\ Fix n >= 0. By the residue form of Lagrange inversion,
\\ [x^n] R(t(x))
\\ = Res R(t) dx/x^(n+1).
\\ This applies because x = (t-1)*u(t) with u(t)=t^(-d) and u(1)=1, so x is a local uniformizer at t=1.
\\ Using x = (t-1)/t^d, this becomes
\\ [x^n] R(t(x))
\\ = [ (t-1)^n ] R(t) * (dx/dt) * t^(d*(n+1)).
\\ Hence
\\ [x^n] R(t(x))
\\ = [ (t-1)^n ]
\\   t^c*(r+s-s*t)/((d-(d-1)*t)*(1-t+t^m))
\\   * (d-(d-1)*t)/t^(d+1)
\\   * t^(d*(n+1)).
\\ The factor d-(d-1)*t cancels, giving
\\ [x^n] R(t(x))
\\ = [ (t-1)^n ] t^(c+d*n-1)*(r+s-s*t)/(1-t+t^m).     (1)
\\
\\ Since y = t-1, we have
\\ r+s-s*t = r-s*y,
\\ 1-t+t^m = t^m-y.
\\ So (1) becomes
\\ [x^n] R(t(x))
\\ = [y^n] t^(c+d*n-1)*(r-s*y)/(t^m-y).               (2)
\\
\\ Expand
\\ 1/(t^m-y) = t^(-m)/(1-y/t^m) = Sum_{j>=0} y^j * t^(-m*(j+1)).
\\ Substituting into (2), we get
\\ [x^n] R(t(x))
\\ = [y^n] (r-s*y) * Sum_{j>=0} y^j * t^(c+d*n-m*(j+1)-1).
\\ Therefore
\\ [x^n] R(t(x))
\\ = Sum_{j=0..n}
\\   ( r*[y^(n-j)] t^(c+d*n-m*j-m-1)
\\   - s*[y^(n-j-1)] t^(c+d*n-m*j-m-1) ).
\\
\\ Now set k = n-j. Then k runs from 0 to n, and since d = l+m,
\\ c+d*n-m*(n-k)-m-1 = c+l*n+m*k-m-1.
\\ Hence
\\ [x^n] R(t(x))
\\ = Sum_{k=0..n}
\\   ( r*[y^k] t^(c+l*n+m*k-m-1)
\\   - s*[y^(k-1)] t^(c+l*n+m*k-m-1) ).
\\
\\ Since t = 1+y, we have for every integer N
\\ [y^k] t^N = binomial(N,k),
\\ [y^(k-1)] t^N = binomial(N,k-1),
\\ where binomial(N,k)=0 for k<0 by convention.
\\ Therefore
\\ [x^n] R(t(x))
\\ = Sum_{k=0..n}
\\   ( r*binomial(l*n+m*k-m-1+c,k)
\\   - s*binomial(l*n+m*k-m-1+c,k-1) )
\\ = a_{l,m,c,r,s}(n).
\\
\\ Therefore, for all integers l, m, c in Z,
\\ A_{l,m,c,r,s}(x)
\\ = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m)),
\\ where t = t(x) is the unique formal power series with constant term 1 satisfying
\\ t = 1 + x*t^(l+m).

default(parisize, 200000000);

N = 12;

\\ Formal-power-series convention.
C(n, k) = if(k >= 0, binomial(n, k));

\\ Candidate formula:
\\ a_{l,m,c,r,s}(n) = Sum_{k=0..n}
\\   (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1))
\\ A(x) = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m)),
\\ where t = 1 + x*t^(l+m).

check_case(l,m,c,r,s)=
{
  my(t, A, ok = 1);
  t = 1 + x + O(x^(N+1));
  for(i=1, N+8, t = 1 + x*t^(l+m) + O(x^(N+1)));
  A = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m));
  print("--------------------------------------------------");
  print(["CASE", l, m, c, r, s]);
  print(["t(x)", t]);
  print(["A(x)", A + O(x^(N+1))]);
  for(n=0, N,
    my(lhs = sum(k=0,n,
          r*C(l*n + m*k - m - 1 + c, k)
          - s*C(l*n + m*k - m - 1 + c, k-1)
        ),
       rhs = polcoeff(A,n),
       good = if(lhs == rhs, "ok", "fail"));
    if(good == "fail", ok = 0);
    print([n, lhs, rhs, good]);
  );
  print(["ALL_OK", if(ok, "ok", "fail")]);
  ok
};

batch_check(l0,l1,m0,m1,c0,c1,r0,r1,s0,s1)=
{
  my(total = 0, bad = 0);
  for(l=l0,l1,
    for(m=m0,m1,
      for(c=c0,c1,
        for(r=r0,r1,
          for(s=s0,s1,
            my(t, A, ok = 1);
            total++;
            t = 1 + x + O(x^(N+1));
            for(i=1, N+8, t = 1 + x*t^(l+m) + O(x^(N+1)));
            A = t^c*(r+s-s*t)/((l+m-(l+m-1)*t)*(1-t+t^m));
            for(n=0, N,
              my(lhs = sum(k=0,n,
                    r*C(l*n + m*k - m - 1 + c, k)
                    - s*C(l*n + m*k - m - 1 + c, k-1)
                  ),
                 rhs = polcoeff(A,n));
              if(lhs != rhs, ok = 0; break);
            );
            if(!ok,
              bad++;
              print(["FAIL_CASE", l, m, c, r, s]);
            );
          );
        );
      );
    );
  );
  print(["BATCH_DONE", "total", total, "bad", bad, "ok", total-bad]);
};

print(["PARI_version", version()]);
\\ print(["binomial(-1,-1)", binomial(-1,-1)]);
\\ print(["binomial(-1,0)", binomial(-1,0)]);
\\ print(["binomial(-1,1)", binomial(-1,1)]);
\\ print(["binomial(0,-1)", binomial(0,-1)]);

\\ This is the special case l=3, m=2, c=3, r=1, s=0 of the following family.
check_case(3,2,3,1,0);

\\ Boundary cases with m=0.
\\ check_case(1,0,1,1,1);
\\ check_case(1,0,0,2,3);
\\ check_case(2,0,0,1,1);

\\ Boundary cases with l=0.
\\ check_case(0,1,0,1,1);
\\ check_case(0,1,2,4,1);

\\ Extreme edge with l+m=1.
\\ check_case(1,0,2,5,5);

\\ Small exhaustive checks. Adjust ranges as desired.
\\ batch_check(0,3,0,3,-4,4,1,3,1,3);
\\ batch_check(-3,3,-3,3,-4,4,-2,2,-2,2);
