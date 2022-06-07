def search(a, num, n)
  if num == n + 1
    @cnt += 1
  else
    (1..n).each{|i|
      if a[i] == 0
        if i == 1 || i.gcd(num) > 1
          a[i] = num
          search(a, num + 1, n)
          a[i] = 0
        end
      end
    }
  end
end

def A(n)
  a = [0] * (n + 1)
  @cnt = 0
  search(a, 1, n)
  @cnt
end

def A354830(n)
  (0..n).map{|i| A(i)}
end

p A354830(15)