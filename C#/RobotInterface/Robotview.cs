using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace RobotInterface
{
    /// <summary>
    /// Vue 2D du robot : repère en mètres, X vers la droite, Y vers le haut, θ en radians (anti-horaire).
    /// Zoom : molette. Déplacement : clic droit/milieu + glisser. Clic gauche : événement WaypointClicked.
    /// </summary>
    public class RobotView : FrameworkElement
    {
        // ---------- Paramètres ----------
        public const int MaxTrailPoints = 3000;
        const double RobotLength = 0.26;   // m
        const double RobotWidth = 0.22;   // m (voie ≈ 0.218)

        // ---------- État ----------
        readonly List<Point> robotTrail = new List<Point>();
        readonly List<Point> ghostTrail = new List<Point>();
        Point robotPos, ghostPos, waypoint;
        double robotTheta, ghostTheta;
        bool hasRobot, hasGhost, hasWaypoint;

        double scale = 120;                 // pixels par mètre
        Point origin = new Point(double.NaN, double.NaN); // position écran de (0,0)
        bool panning;
        Point panStart, originStart;

        public bool FollowRobot { get; set; } = false;

        /// <summary>Levé quand l'utilisateur clique : coordonnées monde (m).</summary>
        public event Action<double, double> WaypointClicked;

        // ---------- Couleurs ----------
        static readonly Brush BgBrush = Frozen(new SolidColorBrush(Color.FromRgb(0x14, 0x17, 0x1c)));
        static readonly Brush TextBrush = Frozen(new SolidColorBrush(Color.FromRgb(0x7d, 0x86, 0x94)));
        static readonly Pen MinorPen = FrozenPen(Color.FromRgb(0x1f, 0x24, 0x2c), 1);
        static readonly Pen MajorPen = FrozenPen(Color.FromRgb(0x2c, 0x33, 0x3e), 1);
        static readonly Color RobotColor = Color.FromRgb(0x4d, 0xa3, 0xff);
        static readonly Color GhostColor = Color.FromRgb(0xff, 0xb4, 0x3b);
        static readonly Color WayColor = Color.FromRgb(0x4a, 0xde, 0x80);

        static Brush Frozen(SolidColorBrush b) { b.Freeze(); return b; }
        static Pen FrozenPen(Color c, double t) { var p = new Pen(new SolidColorBrush(c), t); p.Freeze(); return p; }

        public RobotView()
        {
            ClipToBounds = true;
            Focusable = true;
        }

        // ---------- API publique (appeler depuis le thread UI : Dispatcher.Invoke) ----------
        public void UpdateRobot(double x, double y, double theta)
        {
            robotPos = new Point(x, y); robotTheta = theta; hasRobot = true;
            Push(robotTrail, robotPos, 0.003);
            if (FollowRobot) CenterOn(robotPos);
            InvalidateVisual();
        }

        public void UpdateGhost(double x, double y, double theta)
        {
            ghostPos = new Point(x, y); ghostTheta = theta; hasGhost = true;
            Push(ghostTrail, ghostPos, 0.003);
            InvalidateVisual();
        }

        /// <summary>Met à jour uniquement le cap du ghost (message 0x84).</summary>
        public void UpdateGhostTheta(double theta) { ghostTheta = theta; InvalidateVisual(); }

        public void SetWaypoint(double x, double y)
        {
            waypoint = new Point(x, y); hasWaypoint = true; InvalidateVisual();
        }

        public void ClearTrails() { robotTrail.Clear(); ghostTrail.Clear(); hasWaypoint = false; InvalidateVisual(); }

        public void ResetView()
        {
            scale = 120; origin = new Point(double.NaN, double.NaN); InvalidateVisual();
        }

        static void Push(List<Point> l, Point p, double minStep)
        {
            if (l.Count > 0)
            {
                var last = l[l.Count - 1];
                if (Math.Abs(last.X - p.X) < minStep && Math.Abs(last.Y - p.Y) < minStep) return;
            }
            l.Add(p);
            if (l.Count > MaxTrailPoints) l.RemoveAt(0);
        }

        // ---------- Conversions ----------
        Point ToScreen(Point w) => new Point(origin.X + w.X * scale, origin.Y - w.Y * scale);
        Point ToWorld(Point s) => new Point((s.X - origin.X) / scale, (origin.Y - s.Y) / scale);

        void CenterOn(Point w)
        {
            if (ActualWidth <= 0) return;
            origin = new Point(ActualWidth / 2 - w.X * scale, ActualHeight / 2 + w.Y * scale);
        }

        // ---------- Interaction ----------
        protected override void OnMouseWheel(MouseWheelEventArgs e)
        {
            var m = e.GetPosition(this);
            var before = ToWorld(m);
            scale = Math.Max(15, Math.Min(2000, scale * (e.Delta > 0 ? 1.15 : 1 / 1.15)));
            origin = new Point(m.X - before.X * scale, m.Y + before.Y * scale);
            InvalidateVisual();
        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            Focus();
            if (e.ChangedButton == MouseButton.Right || e.ChangedButton == MouseButton.Middle)
            {
                panning = true; panStart = e.GetPosition(this); originStart = origin;
                CaptureMouse(); Cursor = Cursors.SizeAll;
            }
            else if (e.ChangedButton == MouseButton.Left)
            {
                var w = ToWorld(e.GetPosition(this));
                SetWaypoint(w.X, w.Y);
                WaypointClicked?.Invoke(w.X, w.Y);
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (!panning) return;
            var p = e.GetPosition(this);
            origin = new Point(originStart.X + p.X - panStart.X, originStart.Y + p.Y - panStart.Y);
            InvalidateVisual();
        }

        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            if (!panning) return;
            panning = false; ReleaseMouseCapture(); Cursor = Cursors.Arrow;
        }

        // ---------- Rendu ----------
        protected override void OnRender(DrawingContext dc)
        {
            double w = ActualWidth, h = ActualHeight;
            dc.DrawRectangle(BgBrush, null, new Rect(0, 0, w, h));
            if (w < 10 || h < 10) return;

            if (double.IsNaN(origin.X)) origin = new Point(w / 2, h / 2);

            DrawGrid(dc, w, h);
            DrawTrail(dc, ghostTrail, GhostColor, 0.55, 2);
            DrawTrail(dc, robotTrail, RobotColor, 0.9, 2.5);
            if (hasWaypoint) DrawWaypoint(dc);
            if (hasRobot && hasGhost) DrawError(dc);
            if (hasGhost) DrawRobot(dc, ghostPos, ghostTheta, GhostColor, ghost: true);
            if (hasRobot) DrawRobot(dc, robotPos, robotTheta, RobotColor, ghost: false);
            DrawScaleBar(dc, w, h);
        }

        void DrawGrid(DrawingContext dc, double w, double h)
        {
            // pas "joli" : 1-2-5 × 10^n tel que ≥ 60 px
            double target = 60 / scale;
            double pow = Math.Pow(10, Math.Floor(Math.Log10(target)));
            double step = pow;
            foreach (var m in new[] { 1.0, 2.0, 5.0, 10.0 }) { step = pow * m; if (step >= target) break; }
            double minor = step / 5;

            var tl = ToWorld(new Point(0, 0));
            var br = ToWorld(new Point(w, h));

            if (minor * scale >= 10)
            {
                for (double x = Math.Floor(tl.X / minor) * minor; x <= br.X; x += minor)
                { double sx = Math.Round(ToScreen(new Point(x, 0)).X) + .5; dc.DrawLine(MinorPen, new Point(sx, 0), new Point(sx, h)); }
                for (double y = Math.Floor(br.Y / minor) * minor; y <= tl.Y; y += minor)
                { double sy = Math.Round(ToScreen(new Point(0, y)).Y) + .5; dc.DrawLine(MinorPen, new Point(0, sy), new Point(w, sy)); }
            }

            for (double x = Math.Floor(tl.X / step) * step; x <= br.X; x += step)
            {
                double sx = Math.Round(ToScreen(new Point(x, 0)).X) + .5;
                dc.DrawLine(MajorPen, new Point(sx, 0), new Point(sx, h));
                if (Math.Abs(x) > 1e-9) Label(dc, Fmt(x), new Point(sx + 3, h - 16));
            }
            for (double y = Math.Floor(br.Y / step) * step; y <= tl.Y; y += step)
            {
                double sy = Math.Round(ToScreen(new Point(0, y)).Y) + .5;
                dc.DrawLine(MajorPen, new Point(0, sy), new Point(w, sy));
                if (Math.Abs(y) > 1e-9) Label(dc, Fmt(y), new Point(4, sy - 14));
            }

            // axes
            var o = ToScreen(new Point(0, 0));
            var axX = FrozenPen(Color.FromRgb(0xe5, 0x5b, 0x5b), 1.5);
            var axY = FrozenPen(Color.FromRgb(0x5b, 0xe5, 0x8a), 1.5);
            dc.DrawLine(axX, new Point(o.X, o.Y), new Point(o.X + 40, o.Y));
            dc.DrawLine(axY, new Point(o.X, o.Y), new Point(o.X, o.Y - 40));
            Label(dc, "x", new Point(o.X + 43, o.Y - 8));
            Label(dc, "y", new Point(o.X - 4, o.Y - 58));
        }

        static string Fmt(double v) => v.ToString("0.##", CultureInfo.InvariantCulture) + " m";

        void Label(DrawingContext dc, string s, Point p)
        {
            var ft = new FormattedText(s, CultureInfo.InvariantCulture, FlowDirection.LeftToRight,
                new Typeface("Segoe UI"), 11, TextBrush, VisualTreeHelper.GetDpi(this).PixelsPerDip);
            dc.DrawText(ft, p);
        }

        void DrawTrail(DrawingContext dc, List<Point> pts, Color c, double opacity, double thickness)
        {
            if (pts.Count < 2) return;
            var geo = new StreamGeometry();
            using (var ctx = geo.Open())
            {
                ctx.BeginFigure(ToScreen(pts[0]), false, false);
                for (int i = 1; i < pts.Count; i++) ctx.LineTo(ToScreen(pts[i]), true, true);
            }
            geo.Freeze();
            var b = new SolidColorBrush(c) { Opacity = opacity };
            var pen = new Pen(b, thickness) { LineJoin = PenLineJoin.Round, StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };
            dc.DrawGeometry(null, pen, geo);
        }

        void DrawWaypoint(DrawingContext dc)
        {
            var p = ToScreen(waypoint);
            var pen = new Pen(new SolidColorBrush(WayColor), 2);
            dc.DrawEllipse(null, pen, p, 10, 10);
            dc.DrawLine(pen, new Point(p.X - 16, p.Y), new Point(p.X + 16, p.Y));
            dc.DrawLine(pen, new Point(p.X, p.Y - 16), new Point(p.X, p.Y + 16));
            Label(dc, $"({waypoint.X:0.00} ; {waypoint.Y:0.00})", new Point(p.X + 14, p.Y + 10));
        }

        void DrawError(DrawingContext dc)
        {
            var a = ToScreen(robotPos); var b = ToScreen(ghostPos);
            var pen = new Pen(new SolidColorBrush(Color.FromRgb(0xe5, 0x5b, 0x5b)), 1.5) { DashStyle = DashStyles.Dot };
            dc.DrawLine(pen, a, b);
            double d = (ghostPos - robotPos).Length;
            Label(dc, $"{d * 100:0.0} cm", new Point((a.X + b.X) / 2 + 6, (a.Y + b.Y) / 2 - 16));
        }

        void DrawRobot(DrawingContext dc, Point worldPos, double theta, Color c, bool ghost)
        {
            var s = ToScreen(worldPos);
            double L = RobotLength * scale, W = RobotWidth * scale;

            dc.PushTransform(new TranslateTransform(s.X, s.Y));
            dc.PushTransform(new RotateTransform(-theta * 180.0 / Math.PI)); // axe y écran inversé

            var fill = new SolidColorBrush(c) { Opacity = ghost ? 0.12 : 0.30 };
            var pen = new Pen(new SolidColorBrush(c), ghost ? 1.5 : 2.2);
            if (ghost) pen.DashStyle = new DashStyle(new double[] { 3, 3 }, 0);

            // corps
            dc.DrawRoundedRectangle(fill, pen, new Rect(-L / 2, -W / 2, L, W), 4, 4);

            // roues
            if (!ghost)
            {
                var wheel = new SolidColorBrush(Color.FromRgb(0xe6, 0xe9, 0xee));
                dc.DrawRoundedRectangle(wheel, null, new Rect(-L * 0.18, -W / 2 - 3, L * 0.36, 6), 2, 2);
                dc.DrawRoundedRectangle(wheel, null, new Rect(-L * 0.18, W / 2 - 3, L * 0.36, 6), 2, 2);
            }

            // flèche de cap
            var arrow = new StreamGeometry();
            using (var ctx = arrow.Open())
            {
                ctx.BeginFigure(new Point(L * 0.62, 0), true, true);
                ctx.LineTo(new Point(L * 0.18, -W * 0.28), true, false);
                ctx.LineTo(new Point(L * 0.18, W * 0.28), true, false);
            }
            arrow.Freeze();
            dc.DrawGeometry(new SolidColorBrush(c), null, arrow);

            dc.Pop(); dc.Pop();
        }

        void DrawScaleBar(DrawingContext dc, double w, double h)
        {
            double meters = scale > 400 ? 0.1 : scale > 60 ? 0.5 : 1;
            double px = meters * scale;
            var pen = new Pen(TextBrush, 2);
            var a = new Point(w - 20 - px, h - 22);
            dc.DrawLine(pen, a, new Point(a.X + px, a.Y));
            dc.DrawLine(pen, new Point(a.X, a.Y - 4), new Point(a.X, a.Y + 4));
            dc.DrawLine(pen, new Point(a.X + px, a.Y - 4), new Point(a.X + px, a.Y + 4));
            Label(dc, meters.ToString("0.#", CultureInfo.InvariantCulture) + " m", new Point(a.X + px / 2 - 12, a.Y - 20));
        }
    }
}