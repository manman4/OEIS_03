N=55;
M=50;

\\ G.f.: (C(x)/x)^(1/3), where C(x) is the g.f. of A396209.
B(v) = v / (1 - v)^3;
A = (B(B(B(x + x*O(x^N))))/x)^(1/3);

\\ for(n=0, M, write("b393908.txt", n, " ", polcoef(A, n)));
Vec(A)

\\ G.f.: ((1-x) * ((1-x)^3 - x))^2 / (((1-x)^3 - x)^3 - x*(1-x)^6).
Vec( A - ((1-x) * ((1-x)^3 - x))^2 / (((1-x)^3 - x)^3 - x*(1-x)^6) )

\\ G.f.: 1 / ( (1-x) * (1-B(x)) * (1-B(B(x))) ), where B(x) = x/(1-x)^3.
A1 = B(x + x*O(x^N));
A2 = B(B(x + x*O(x^N)));
Vec(1/( (1-x) * (1-A1) * (1-A2) ))