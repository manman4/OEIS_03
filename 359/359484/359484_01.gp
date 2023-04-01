a(n) = {
    v=[1, -3];
    if(n<3, 
        v[n],
        -n * sumdiv(n, d, if(d<n, a(d) / d))
    )
}

for(n=1, 30, print1(a(n),", ")) 
