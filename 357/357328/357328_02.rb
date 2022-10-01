def search(a, num, n)
  if num == n + 1
    # p [n, a]
    @count += 1
  else
    ((0..n).to_a - a[0..num - 1]).each{|i|
      flag = true
      (1..num - 1).each{|j|
        if num % j == 0
          flag = false if i % a[j] > 0
        end
      }
      if flag
        a[num] = i
        search(a, num + 1, n)
        a[num] = 0
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

p (1..30).map{|i| p A(i)}
