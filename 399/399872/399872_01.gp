default(realprecision, 200); /* using floats */
sn(n, k)=1/n*sum(i=1, k, sumdiv(n, j, eulerphi(j)*(1+2*cos(i*Pi/(k+1)))^(n/j)));
vn=vector(66, n, round(sn(n, 6)) ) /* necklaces */
\\ a(n) = Sum_{d|n} mu(n/d) * A208775(d).
vl=vector(#vn, n, sumdiv(n, d, moebius(n/d)*vn[d]))