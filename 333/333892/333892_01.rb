def search(a, num, n, prod)
  if num == n + 1
    #p a
    @count += 1
  else
    (1..n).each{|i|
      if a[i] == 0
        prod *= i
        if prod % num == 0
          
          a[i] = num
          #p [prod, a]
          search(a, num + 1, n, prod)
          a[i] = 0
        end
        prod /= i
      end
    }
  end
end
def A(n)
  a = [0] * (n + 1)
  @count = 0
  search(a, 1, n, 1)
  @count
end



p (1..21).map{|i| p A(i)}
def B(n)
  s = 0
  (1..n).to_a.permutation{|c|
    s += 1 if (2..n).all?{|i| c[0..i - 1].inject(:*) % i == 0}
  }
  s
end

(1..10).each{|i| p B(i)}
