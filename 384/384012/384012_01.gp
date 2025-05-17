c(n) = {
    my(sum = 0);
    for(i=0, n,
        for(j=0, n,
            for(k=0, n,
                if(i + j + k == 2*n,
                    sum += abs(stirling(n+1, i+1, 1) * stirling(n+1, j+1, 1) * stirling(n+1, k+1, 1));
                )
            )
        )
    );
    return(sum);
};

for(n=0, 20, print1(c(n),", "));
