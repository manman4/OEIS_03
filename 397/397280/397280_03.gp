M=20;

\\ my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+1-exp(x))/x))



\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x * A(x))^2) - 1)/x.
my(A=1, N=M); for(k=1, N, A=1+(exp((x*A)^2)-1)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+1-exp(x^2))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/2)..n} (2*k)!/k! * Stirling2(k, 2*k-n).
a(n) = (1/(n+1)) * sum(k=ceil(n/2), n, (2*k)!/k! * stirling(k, 2*k-n, 2));
for(n=0, M, print1(a(n), ", "));



\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x * A(x))^3) - 1)/x.
my(A=1, N=M); for(k=1, N, A=1+(exp((x*A)^3)-1)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+1-exp(x^3))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/3)..floor(n/2)} (3*k)!/k! * Stirling2(k, 3*k-n).
a(n) = (1/(n+1)) * sum(k=ceil(n/3), n\2, (3*k)!/k! * stirling(k, 3*k-n, 2));
for(n=0, M, print1(a(n), ", "));



\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x * A(x))^4) - 1)/x.
my(A=1, N=M); for(k=1, N, A=1+(exp((x*A)^4)-1)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+1-exp(x^4))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/4)..floor(n/3)} (4*k)!/k! * Stirling2(k, 4*k-n).
a(n) = (1/(n+1)) * sum(k=ceil(n/4), n\3, (4*k)!/k! * stirling(k, 4*k-n, 2));
for(n=0, M, print1(a(n), ", "));


print1("...  ...", "\n");



\\ E.g.f. A(x) satisfies A(x) = 1 - log(1 - (x * A(x))^2)/x.
my(A=1, N=M); for(k=1, N, A=1-log(1-(x*A)^2)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+log(1-x^2))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/2)..n} (2*k)!/k! * |Stirling1(k, 2*k-n)|.
a(n) = (1/(n+1)) * sum(k=ceil(n/2), n, (2*k)!/k! * abs(stirling(k, 2*k-n, 1)));
for(n=0, M, print1(a(n), ", "));



\\ E.g.f. A(x) satisfies A(x) = 1 - log(1 - (x * A(x))^3)/x.
my(A=1, N=M); for(k=1, N, A=1-log(1-(x*A)^3)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+log(1-x^3))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/3)..floor(n/2)} (3*k)!/k! * |Stirling1(k, 3*k-n)|.
a(n) = (1/(n+1)) * sum(k=ceil(n/3), n\2, (3*k)!/k! * abs(stirling(k, 3*k-n, 1)));
for(n=0, M, print1(a(n), ", "));



\\ E.g.f. A(x) satisfies A(x) = 1 - log(1 - (x * A(x))^4)/x.
my(A=1, N=M); for(k=1, N, A=1-log(1-(x*A)^4)/x+x*O(x^N)); Vec(serlaplace(A))

my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(x+log(1-x^4))/x))

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/4)..floor(n/3)} (4*k)!/k! * |Stirling1(k, 4*k-n)|.
a(n) = (1/(n+1)) * sum(k=ceil(n/4), n\3, (4*k)!/k! * abs(stirling(k, 4*k-n, 1)));
for(n=0, M, print1(a(n), ", "));