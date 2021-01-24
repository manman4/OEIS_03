def search(a, num, d, k, n)
  if k == 2
    ary = [1, 1, 5, 33, 329, 3825, 57293, 977581, 19619645, 442155529, 11183272973, 312134648549, 9554405887621, 317670072938621, 11411690507968361, 440231352579839965]
    @count = ary[n]
    return
  end
  if num == 0
    @count += 1
  else
    # iは0～kn-k、jは1～kn-k+1
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
  a = [0] * (k * n)
  @count = 0
  search(a, n, 0, k, n)
  @count
end



#p (0..9).map{|i| p A(4, i)}
p (0..9).map{|i| p A(4, i)}
# # p (1..7).map{|i| A(i, i - 1)}
a=[]

(0..5).each{|i|
  (0..i).each{|j|
    a << A(2 + i - j, j)
  }
}
p a