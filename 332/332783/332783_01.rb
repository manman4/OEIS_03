def search(a, num, d, k, n)
  if num == 0
    @cnt += 1
  else
    (k * n - k + 1).times{|i|
      if a[i] == 0
        (i + d + 1..k * n - k + 1).each{|j|
          if (k - 1) * j - (k - 2) * i < k * n
            if (1..k - 1).all?{|m| a[m * j - (m - 1) * i] == 0}
              (0..k - 1).each{|m| a[m * j - (m - 1) * i] = num}
              search(a, num - 1, j - i - 1, k, n)
              (0..k - 1).each{|m| a[m * j - (m - 1) * i] = 0}
            end
          end
        }
      end
    }
  end
end

def A(k, n)
  a = [0] * k * n
  @cnt = 0
  search(a, n, 0, k, n)
  @cnt
end

def A332783(n)
  (1..n).map{|i| A(i + 1, i)}
end

p A332783(5)
