def search(a, num, n)
  if num == n + 1
    @count += 1
  else
    (1..n).each{|i|
      if a[i] == 0
        if num % i == 0 || i % num == 0
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
  @count = 0
  search(a, 1, n)
  @count
end

(21..30).each{|i| p A(i)}
