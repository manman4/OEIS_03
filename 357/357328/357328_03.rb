require 'prime'

def search(a, num, n)
  if num == n + 1
    # p [n, a]
    @count += 1
  elsif a[num] > 0
    search(a, num + 1, n)
  else
    ((0..n).to_a - a).each{|i|
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
  p_ary = Prime.each(Math.sqrt(n).to_i).to_a
  a = [0, 1] + [0] * (n - 1)
  (2..n).each{|i|
    a[i] = i if i.prime_division.map{|j| j[0]}.all?{|j| p_ary.include?(j)}
  }
  @count = 0
  search(a, 1, n)
  @count
end

p (1..40).map{|i| p A(i)}
