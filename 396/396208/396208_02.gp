N=55;
M=50;

\\ G.f.: (B(x)/x)^(1/4), where B(x) is the g.f. of A396210.
B(v) = v / (1 - v)^4;
A = (B(B(B(x + x*O(x^N))))/x)^(1/4);

\\ for(n=0, M, write("b396208.txt", n, " ", polcoef(A, n)));

Vec(A)

\\ G.f.: ((1-x) * ((1-x)^4 - x))^3 / (((1-x)^4 - x)^4 - x*(1-x)^12).
Vec( A - ( ((1-x) * ((1-x)^4 - x))^3 / (((1-x)^4 - x)^4 - x*(1-x)^12) ) )