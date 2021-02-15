#filename = "slowturn_2.log"
##filename = "slowturn_6.log" # keep
##filename = "slowturn_7.log"
##filename = "crash.log" # keep

#filename = "turn_and_stop.log"
filename = "one_turn.log"
#filename = "multiple_turns.log"

a = read.delim(filename)

x = a$time
if (is.null(x)) {
    x = (1:nrow(a))/20
} else {
    x = (a$time - min(a$time))/1000
}
plot(x, a$tz - min(a$tz), type="b", main=filename, sub=median(a$right/a$left))

if ("tag" %in% names(a)) {
    sel = a$tag != ""
    abline(v=x[sel], lty=3)
}

lines(x, a$left, type="b", col="green")
lines(x, a$right, type="b", col="blue")

lines(x, a$cm*1, type="b", col="red")
abline(h=200, lty=2)

sel = min(which(a$tag != ""))
tz = a$tz - a$tz[sel]

theta = tz * pi / 180 / 2

xl = sin(theta) * a$left
yl = cos(theta) * a$left
xr = sin(theta) * a$right + 56
yr = cos(theta) * a$right + 56

xb = sin(theta) * mean(a$left + a$right)/2
yb = cos(theta) * mean(a$left + a$right)/2

xd = sin(theta) * a$cm * 10
yd = cos(theta) * a$cm * 10

plot(xb, yb, type="l", col="grey", lty=2, xlim=c(-1,1)*1000, ylim=c(-1,1)*1000)
lines(xl, yl, type="l", col="green", cex=0.3)
lines(xr, yr, type="l", col="blue", cex=0.3)
lines(xd, yd, type="l", col="red", cex=0.3)

