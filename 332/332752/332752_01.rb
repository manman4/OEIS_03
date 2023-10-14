def search(a, num, d, k, n)
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

p (0..8).map{|i| p A(4, i)}

# def search(a, num, d, k, n)
#   if num == 0
#     @count += 1
#   else
#     (4 * n - 3).times{|i|
#       if a[i] == 0
#         (i + d + 1..4 * n - 1).each{|j|
#           if a[j] == 0 && (3 * j - 2 * i < 4 * n)
#             if a[2 * j - i] == 0 && a[3 * j - 2 * i] == 0
#               a[i], a[j], a[2 * j - i], a[3 * j - 2 * i] = num, num, num, num
#               search(a, num - 1, j - i - k, k, n)
#               a[i], a[j], a[2 * j - i], a[3 * j - 2 * i] = 0, 0, 0, 0
#             end
#           end
#         }
#       end
#     }
#   end
# end

# def A(k, n)
#   a = [0] * n * 4
#   @count = 0
#   search(a, n, 0, k, n)
#   @count
# end

# puts "A261517"
# p (0..15).map{|i| p A(0, i)}
# puts "A322178"
# p (0..10).map{|i| p A(1, i)}