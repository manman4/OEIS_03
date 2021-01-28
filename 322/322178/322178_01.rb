def search(a, n, num, d)
  if num == 0
    @cnt += 1
  else
    (2 * n - 1).times{|i|
      if a[i] == 0
        (i + d + 1..2 * n - 1).each{|j|
          if a[j] == 0
            a[i], a[j] = num, num
            search(a, n, num - 1, j - i - 1)
            a[i], a[j] = 0, 0
          end
        }
      end
    }
  end
end
def A(n)
  a = [0] * n * 2
  @cnt = 0
  search(a, n, n, 0)
  @cnt
end
def A322178(n)
  (1..n).each{|i| p A(i)}
end
A322178(9)
