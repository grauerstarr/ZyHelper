using System.Globalization;
using Turbo.Plugins.Default;
using System.Linq;
using SharpDX.DirectInput;

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Collections.Generic;

namespace Turbo.Plugins.Zy
{
    public class ZyHelperBetaAdapter : BasePlugin, IInGameTopPainter, IKeyEventHandler, IAfterCollectHandler
    {

        public TopLabelDecorator logoDecorator { get; set; }
        TcpClient client;
        NetworkStream stream;
        private readonly int[] _skillOrder = { 2, 3, 4, 5, 0, 1 };
        public bool Show { get; set; }
        public IKeyEvent ToggleKeyEvent { get; set; }

        public int Distance { get; set; }
        public int MaxRadius { get; set; }
        public int ping { get; set; }
        public int DisplayLimit { get; set; }
        public int count2 { get; set; }
        public double MRiftProgression { get; set; }
        public float PosiX { get; set; }
        public float PosiY { get; set; }
        public float OcuX { get; set; }
        public float OcuY { get; set; }
        public float MyX { get; set; }
        public float MyY { get; set; }

        public double Latenz { get; set; }
        IWorldCoordinate ShockCoor { get; set; }

        IWorldCoordinate MCoordinate { get; set; }
        int Mcount { get; set; }

        private IWatch riftTimer;
        public Key HotKey
        {
            get { return ToggleKeyEvent.Key; }
            set { ToggleKeyEvent = Hud.Input.CreateKeyEvent(true, value, false, false, false); }
        }
        private StringBuilder textBuilder;
        private IFont Font;
        public ZyHelperBetaAdapter()
        {
            Enabled = true;
        }

        public override void Load(IController hud)
        {
            base.Load(hud);

            string textFunc() => "ZH";
            logoDecorator = new TopLabelDecorator(Hud)
            {
                TextFont = Hud.Render.CreateFont("tahoma", 8, 255, 146, 99, 6, true, false, false),
                TextFunc = textFunc,
            };
            riftTimer = Hud.Time.CreateWatch();
            HotKey = Key.F11;
            Show = false;
            Font = Hud.Render.CreateFont("tahoma", 8, 255, 146, 99, 6, true, false, false);
            textBuilder = new StringBuilder();

            int port = 2206;
            client = new TcpClient("localhost", port);
            if (Enabled) stream = client.GetStream();


            byte[] myWriteBuffer = Encoding.ASCII.GetBytes("Starting connection");
            if (Enabled) stream.Write(myWriteBuffer, 0, myWriteBuffer.Length);
            if (Enabled) stream.Flush();
        }

        public void OnKeyEvent(IKeyEvent keyEvent)
        {
            if (keyEvent.IsPressed && ToggleKeyEvent.Matches(keyEvent))
            {
                Show = !Show;
            }
        }
             

        public int GetByteFromFloat(double fdata, ref Byte data0, ref Byte data1, ref Byte data2, ref Byte data3)
        { //Get data0, data1, data2, data3 from IEEE 754 Floating point format //fdata : float data //data0, data1, data2, data3 : 32-bit Hexadecimal data     //return code ==> 0: OK, -1: Not Available 
            byte[] arr = new byte[4];
            float value = (float)fdata;
            arr = BitConverter.GetBytes(value);
            data0 = arr[3];
            data1 = arr[2];
            data2 = arr[1];
            data3 = arr[0];
            return 0;
        }


        public static Byte Set(byte input, int pos, bool value)
        {
            Byte result = input;
            if (value)
            {
                result = (byte)(input | (1 << pos));
            }
            else
            {
                result = (byte)(input & ~(1 << pos));
            }
            return result;
        }

        public void PaintTopInGame(ClipState clipState)
        {
            float x = -Hud.Window.Size.Width * 0.001f;
            float y = Hud.Window.Size.Height * 0.965f;

            logoDecorator.Paint(x, y, 30.0f, 30.0f, HorizontalAlign.Center);

            

            if (Show)
            {
                float XPos = Hud.Window.Size.Width * 0.01f;
                float YPos = Hud.Window.Size.Height * 0.1f;
                textBuilder.Clear();

                textBuilder.AppendFormat("IsInGame: {0}", Hud.Game.IsInGame);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("IsPaused: {0}", Hud.Game.IsPaused);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("IsLoading: {0}", Hud.Game.IsLoading);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("IsDead: {0}", Hud.Game.Me.IsDead);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("UiHidden: {0}", Hud.Render.UiHidden);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("Minimap: {0}", Hud.Game.MapMode == MapMode.Minimap);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("dead or portal: {0}", (Hud.Game.Me.AnimationState != AcdAnimationState.CastingPortal && Hud.Game.Me.AnimationState != AcdAnimationState.Dead));
                textBuilder.AppendLine();
                textBuilder.AppendFormat("in rift: {0}", (Hud.Game.SpecialArea == SpecialArea.Rift || Hud.Game.SpecialArea == SpecialArea.GreaterRift));
                textBuilder.AppendLine();

                IUiElement Chat = Hud.Render.GetUiElement("Root.NormalLayer.chatentry_dialog_backgroundScreen.chatentry_content.chat_editline");
                IUiElement Skills = Hud.Render.GetUiElement("Root.NormalLayer.SkillPane_main.LayoutRoot.SkillsList");
                IUiElement GameMenu = Hud.Render.GetUiElement("Root.NormalLayer.gamemenu_dialog.gamemenu_bkgrnd");
                IUiElement LeaderboardsMain = Hud.Render.GetUiElement("Root.NormalLayer.BattleNetLeaderboard_main.LayoutRoot.OverlayContainer");
                IUiElement AchievementsMain = Hud.Render.GetUiElement("Root.NormalLayer.BattleNetAchievements_main.LayoutRoot.OverlayContainer");
                IUiElement Inventory = Hud.Render.GetUiElement("Root.NormalLayer.inventory_dialog_mainPage.inventory_button_backpack");
                IUiElement contextMenu = Hud.Render.GetUiElement("Root.TopLayer.ContextMenus.PlayerContextMenu");

                textBuilder.AppendFormat("Chat: {0}", Chat.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("Skills: {0}", Skills.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("GameMenu: {0}", GameMenu.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("LeaderboardsMain: {0}", LeaderboardsMain.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("AchievementsMain: {0}", AchievementsMain.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("Inventory: {0}", Inventory.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("contextMenu: {0}", contextMenu.Visible);
                textBuilder.AppendLine();
                textBuilder.AppendFormat("rift time: {0}", riftTimer.ElapsedMilliseconds);
                textBuilder.AppendLine();
                

                var layout = Font.GetTextLayout(textBuilder.ToString());
                Font.DrawText(layout, XPos, YPos);
            }
        }
        public void AfterCollect()
        {
            var riftQuest = Hud.Game.Quests.FirstOrDefault(q => q.SnoQuest.Sno == 337492) ??
                            Hud.Game.Quests.FirstOrDefault(q => q.SnoQuest.Sno == 382695);
            if (riftQuest == null || (riftQuest != null && riftQuest.State == QuestState.none))
            {
                if (riftTimer.IsRunning || riftTimer.ElapsedMilliseconds > 0)
                {
                    riftTimer.Reset();
                }
            }
            else
            {
                if (!riftTimer.IsRunning && !(Hud.Game.Monsters.Any(m => m.Rarity == ActorRarity.Boss && !m.IsAlive) || (riftQuest.QuestStepId == 34 || riftQuest.QuestStepId == 46)))
                    riftTimer.Start();

                if (Hud.Game.Monsters.Any(m => m.Rarity == ActorRarity.Boss && !m.IsAlive) || (riftQuest.QuestStepId == 34 || riftQuest.QuestStepId == 46) && riftTimer.IsRunning)
                    riftTimer.Stop();
            }

            bool Active = true;
            Byte[] Bytes = new Byte[8];
            for (int i = 0; i < Bytes.Length; i++)
            {
                Bytes[i] = 0;
                Bytes[i] = Set(Bytes[i], 0, true);
            }
            Active = Active && Hud.Game.IsInGame;
            Active = Active && !Hud.Game.IsLoading;
            Active = Active && !Hud.Game.IsInTown;
            Active = Active && !Hud.Game.Me.IsDead;
            Active = Active && !Hud.Render.UiHidden;
            Active = Active && Hud.Game.MapMode == MapMode.Minimap;
            Active = Active && (Hud.Game.Me.AnimationState != AcdAnimationState.CastingPortal && Hud.Game.Me.AnimationState != AcdAnimationState.Dead);
            IUiElement Chat = Hud.Render.GetUiElement("Root.NormalLayer.chatentry_dialog_backgroundScreen.chatentry_content.chat_editline");
            IUiElement Skills = Hud.Render.GetUiElement("Root.NormalLayer.SkillPane_main.LayoutRoot.SkillsList");
            IUiElement GameMenu = Hud.Render.GetUiElement("Root.NormalLayer.gamemenu_dialog.gamemenu_bkgrnd");
            IUiElement LeaderboardsMain = Hud.Render.GetUiElement("Root.NormalLayer.BattleNetLeaderboard_main.LayoutRoot.OverlayContainer");
            IUiElement AchievementsMain = Hud.Render.GetUiElement("Root.NormalLayer.BattleNetAchievements_main.LayoutRoot.OverlayContainer");
            IUiElement Inventory = Hud.Render.GetUiElement("Root.NormalLayer.inventory_dialog_mainPage.inventory_button_backpack");
            IUiElement contextMenu = Hud.Render.GetUiElement("Root.TopLayer.ContextMenus.PlayerContextMenu");

            Active = Active && (!Chat.Visible);
            Active = Active && (!Skills.Visible);
            Active = Active && (!GameMenu.Visible);
            Active = Active && (!LeaderboardsMain.Visible);
            Active = Active && (!AchievementsMain.Visible);
            Active = Active && (!Inventory.Visible);
            Active = Active && (!contextMenu.Visible);

            if (!Active)
            {
                stream.Write(Bytes, 0, Bytes.Length);
                System.Threading.Thread.Sleep(2);
                return;
            }

            PosiX = 0.0f;
            PosiY = 0.0f;
            byte[] BitX = new byte[4];
            byte[] BitY = new byte[4];
            float DBX = 1.0f;
            float DBY = 1.0f;
            byte[] DBBitX = new byte[4];
            byte[] DBBitY = new byte[4];
            OcuX = 0.0f;
            OcuY = 0.0f;
            byte[] OcuBitX = new byte[4];
            byte[] OcuBitY = new byte[4];

            MyX = 0.0f;
            MyY = 0.0f;
            byte[] MyBitX = new byte[4];
            byte[] MyBitY = new byte[4];


            byte[] LatenzBit = new byte[4];


            // 4k Monitor = 1.5f ansonsten auf = 1f setzen
            float scrfaktor = 1f;

            int elitecount = 0;
            int dbcount = 0;
            float breathx = 1.0f;
            float breathy = 1.0f;

            bool shockexist = false;
            bool isVyr = true;

            bool IPOnCooldown = false;
            bool WarCryOnCooldown = false;
            float NecroDistance = 0.0f;
            float WizDistance = 0.0f;
            float MonkDistance = 0.0f;
            float ChargeBarbDistance = 0.0f;
            float Rat1Distance = 0.0f;
            float Rat2Distance = 0.0f;
            bool FalterOnCooldown = false;
            bool BerserkerOnCooldown = false;
            bool ImBarb = false;
            bool ImWizard = false;
            bool ImMonk = false;
            bool ImNecro = false;
            bool EpiphanyOnCooldown = false;
            bool MantraOfHealingOnCooldown = false;
            bool WarCryBuffActive = false;
            bool BerserkerBuffActive = false;
            bool EpiphanyBuffActive = false;
            bool LotDOnCooldown = false;
            bool LotDBuffActive = false;
            bool SprintOnCooldown = false;
            bool SprintBuffActive = false;
            bool WizardIngame = false;
            bool MonkIngame = false;
            int NecrosIngame = 0;
            bool BarbIngame = false;
            bool RecastSweepingWind = false;
            bool BohOnCooldown = false;
            bool BoneArmorAlmostRunningOut = false;
            bool BoneArmorOnCooldown = false;
            bool ConventionLight = false;
            bool ConventionArcane = false;
            bool ConventionCold = false;
            bool ConventionFire = false;
            bool BlackholeBuffActive = false;
            bool StormArmorOnCooldown = false;
            bool StormArmorBuffActive = false;
            bool MagicWeaponOnCooldown = false;
            bool MagicWeaponBuffActive = false;
            bool BossIsSpawned = false;
            bool NatBuffActive = false;
            bool ImDh = false;
            bool ImSader = false;
            bool VengeanceBuffActive = false;
            bool VengeanceOnCooldown = false;
            bool RainOfVengeanceOnCooldown = false;
            bool PreparationOnCooldown = false;
            bool ChilanikBuff = false;
            bool BarbHasValidActor = false;
            int  NumberOfSkeleMages = 0;
            bool EliteInRange = false;
            bool SimBuffActive = false;
            bool SimOnCooldown = false;
            bool DontCastSim = false;
            bool DontCastLand = false;
            bool CastSimInChanneling = false;
            bool InARift = false;
            bool ImZnec = false;
            bool ArchonBuffActive = false;
            bool ArcaneBlastOnCooldown = false;
            bool ExplosiveBlastOnCooldown = false;
            bool BloodNovaOnCooldown = false;
            bool Rat1Dead = false;
            bool Rat2Dead = false;
            double SimCD = 120.0d;
            bool[] IsBuffable = new bool[7];
            bool PartyIsBuffable = true;
            bool CanCast = true;
            bool ZnecLandRunningOut = true;
            bool ChannelingAfterDelay = false;
            bool BlindingFlashActive = false;
            bool BlindingFlashOnCooldown = false;

            bool CastIp = false;
            bool CastWc = false;
            bool CastFalter = false;
            bool CastBerserker = false;
            bool CastSprint = false;
            bool CastEpiphany = false;
            bool CastMantraHealing = false;
            bool CastSweepingWind = false;
            bool CastBoh = false;
            bool CastMantraConviction = false;
            bool CastLotd = false;
            bool CastBoneArmor = false;
            bool CastPotion = false;
            bool CastStormArmor = false;
            bool CastMagicWeapon = false;
            bool CastVengeance = false;
            bool CastRainOfVengeance = false;
            bool CastPreparation = false;
            bool CastSkeleMages = false;
            bool CastSim = false;
            bool ForceMove = false;
            bool CastArcaneBlast = false;
            bool CastExplosiveBlast = false;
			bool CastBloodNova = false;
            bool CastBlindingFlash = false;
            bool CastCommandSkeletons = false;

            bool IpEquipped = false;
            bool WcEquipped = false;
            bool FalterEquipped = false;
            bool BerserkerEquipped = false;
            bool SprintEquipped = false;
            bool EpiphanyEquipped = false;
            bool MantraHealingEquipped = false;
            bool SweepingWindEquipped = false;
            bool BohEquipped = false;
            bool MantraConvictionEquipped = false;
            bool LotdEquipped = false;
            bool BoneArmorEquipped = false;
            bool StormArmorEquipped = false;
            bool MagicWeaponEquipped = false;
            bool VengeanceEquipped = false;
            bool RainOfVengeanceEquipped = false;
            bool PreparationEquipped = false;
            bool SkeleMagesEquipped = false;
            bool SimEquipped = false;
            bool ArchonEquipped = false;
            bool ExplosiveBlastEquippped = false;
            bool BloodNovaEquipped = false;
            bool BlindingFlashEquipped = false;
            bool CommandSkeletonsEquipped = false;

            IWorldCoordinate MyPosition = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate WizPosition = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate MonkPosition = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate NecroPosition = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate ChargeBarbPosition = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate Rat1Position = Hud.Game.Players.First().FloorCoordinate;
            IWorldCoordinate Rat2Position = Hud.Game.Players.First().FloorCoordinate;

            BossIsSpawned = (Hud.Game.AliveMonsters.Count(m => m.SnoMonster.Priority == MonsterPriority.boss) > 0);

            int Range15Enemies = 0;
            int Range25Enemies = 0;
            int Range75Enemies = 0;

            var monsters2 = Hud.Game.AliveMonsters.Where(m => ((m.SummonerAcdDynamicId == 0 && m.IsElite) || !m.IsElite));
            foreach (var monster in monsters2)
            {
                if (monster.FloorCoordinate.XYDistanceTo(Hud.Game.Me.FloorCoordinate) <= 15) Range15Enemies++;
                if (monster.FloorCoordinate.XYDistanceTo(Hud.Game.Me.FloorCoordinate) <= 25) Range25Enemies++;
                if (monster.FloorCoordinate.XYDistanceTo(Hud.Game.Me.FloorCoordinate) <= 75) Range75Enemies++;

                
                if ((monster.Rarity == ActorRarity.Unique) ||
                    (monster.Rarity == ActorRarity.Champion) ||
                    (monster.Rarity == ActorRarity.Rare) ||
                    (monster.SnoMonster.Priority == MonsterPriority.boss) ||
                    (monster.SnoActor.Sno == ActorSnoEnum._x1_pand_ext_ordnance_tower_shock_a))//shocktower
                {
                    if (monster.FloorCoordinate.XYDistanceTo(Hud.Game.Me.FloorCoordinate) <= 70)
                    {
                        EliteInRange = true;
                    }
                }
            }

            var items = Hud.Game.Items;
            foreach (var item in items)
            {
                if ((item.SnoItem.Sno == 2087837753 || item.SnoItem.Sno == 2709165134 || item.SnoItem.Sno == 3689019703 || item.SnoItem.Sno == 3931359676 || item.SnoItem.Sno == 2835237830 || item.IsLegendary) && item.FloorCoordinate.XYZDistanceTo(Hud.Game.Me.FloorCoordinate) <= 10)
                {
                    if (item.SnoItem.Sno == 2087837753)
                    {
                        dbcount += 1;
                        breathx = item.FloorCoordinate.X;
                        breathy = item.FloorCoordinate.Y;
                    }
                    else
                    {
                        var ItemCoor = item.FloorCoordinate;
                        var ScreenCoor = ItemCoor.ToScreenCoordinate();

                        DBX = ScreenCoor.X;
                        DBY = ScreenCoor.Y;
                        DBX *= scrfaktor;
                        DBY *= scrfaktor;
                    }
                }
            }

            if (dbcount > 0)
            {
                DBX = breathx;
                DBY = breathy;
                DBX *= scrfaktor;
                DBY *= scrfaktor;
            }


            foreach (var player in Hud.Game.Players)//me
            {
                if (!player.IsMe) continue;
                MyPosition = player.FloorCoordinate;

                foreach (var i in _skillOrder)
                {
                    var skill = player.Powers.SkillSlots[i];
                    if (skill == null ) continue;
                    //barb
                    if (skill.SnoPower.Sno == 79528)//Barbarian_IgnorePain
                    {
                        IPOnCooldown = skill.IsOnCooldown;
                        IpEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 375483)// Barbarian_WarCry
                    {
                        WarCryBuffActive = skill.BuffIsActive;
                        WarCryOnCooldown = skill.IsOnCooldown;
                        WcEquipped = true;
                        var buff = player.Powers.GetBuff(318821); //ChilaniksChain 318821 - ItemPassive_Unique_Ring_639_x1
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        ChilanikBuff = buff.TimeLeftSeconds[1] > 0.0;
                        
                    }
                    if (skill.SnoPower.Sno == 79077)// Barbarian_ThreateningShout
                    {
                        FalterOnCooldown = skill.IsOnCooldown;
                        FalterEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 79607)// Barbarian_WrathOfTheBerserker
                    {
                        BerserkerBuffActive = skill.BuffIsActive;
                        BerserkerOnCooldown = skill.IsOnCooldown;
                        BerserkerEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 78551)// Barbarian_Sprint
                    {
                        SprintOnCooldown = skill.IsOnCooldown;
                        SprintEquipped = true;
						
						var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        SprintBuffActive = buff.TimeLeftSeconds[0] > 2.0;
                    }

                    //monk
                    if (skill.SnoPower.Sno == 312307)//Monk_Epiphany
                    {
                        EpiphanyEquipped = true;
                        EpiphanyOnCooldown = skill.IsOnCooldown;
                        var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        EpiphanyBuffActive = buff.TimeLeftSeconds[0] > 0.5;
                    }
                    if (skill.SnoPower.Sno == 136954)//Monk_BlindingFlash
                    {
                        BlindingFlashEquipped = true;
                        BlindingFlashOnCooldown = skill.IsOnCooldown;
                        BlindingFlashActive = (Hud.Game.Me.GetAttributeValue(Hud.Sno.Attributes.Power_Buff_1_Visual_Effect_A, 136954) == 1.0);
                    }
                    if (skill.SnoPower.Sno == 373143)//Monk_MantraOfHealing
                    {
                        MantraOfHealingOnCooldown = skill.IsOnCooldown;
                        MantraHealingEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 96090)//Monk_SweepingWind
                    {
                        SweepingWindEquipped = true;
						var buff = skill.Buff;
						if ((buff == null) || (buff.IconCounts[0] <= 0)) 
						{
							RecastSweepingWind = true;
							continue;
						}
						RecastSweepingWind = skill.Buff.TimeLeftSeconds[0] < 1.0;
                        
                    }
                    if (skill.SnoPower.Sno == 69130)//Monk_BreathOfHeaven
                    {
                        BohOnCooldown = skill.IsOnCooldown;
                        BohEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 375088)//Monk_MantraOfConviction
                    {
                        MantraConvictionEquipped = true;
                    }
                    //wizard
                    if (skill.SnoPower.Sno == 74499)//Wizard_StormArmor
                    {
                        StormArmorOnCooldown = skill.IsOnCooldown;
                        StormArmorEquipped = true;
                        var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0))
                            continue;
                        StormArmorBuffActive = buff.TimeLeftSeconds[0] > 60.0;
                    }
                    if (skill.SnoPower.Sno == 76108)//Wizard_MagicWeapon
                    {
                        MagicWeaponBuffActive = skill.BuffIsActive;
                        MagicWeaponOnCooldown = skill.IsOnCooldown;
                        MagicWeaponEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 134872)//Wizard_Archon
                    {
                        var buff = player.Powers.GetBuff(Hud.Sno.SnoPowers.Wizard_Archon.Sno);
                        if (buff != null)
                        {
                            ArchonBuffActive = buff.TimeLeftSeconds[2] > 0.1;
                        }
                        ArchonEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 87525)//Wizard_ExplosiveBlast { get; } // 87525
                    {
                        ExplosiveBlastOnCooldown = skill.IsOnCooldown;
                        ExplosiveBlastEquippped = true;
                    }

                    if (i == 2)
                    {
                        ArcaneBlastOnCooldown = skill.IsOnCooldown;
                    }

                    /*if (skill.SnoPower.Sno == 392885 || skill.SnoPower.Sno == 167355)//Wizard_ArchonArcaneBlastLightning Wizard_ArchonArcaneBlast
                    {
                        ArcaneBlastOnCooldown = skill.IsOnCooldown;
                    }*/


                    //necro
                    if (skill.SnoPower.Sno == 466857)//Necromancer_BoneArmor
                    {
                        BoneArmorOnCooldown = skill.IsOnCooldown;
                        BoneArmorEquipped = true;
                        if (skill.Buff != null)
                        {
                            BoneArmorAlmostRunningOut = skill.Buff.TimeLeftSeconds[0] < 30.0;
                        }
                       
                    }
					if (skill.SnoPower.Sno == 465839)//Necromancer_LandOfTheDead { get; }
                    {
                        //LotDBuffActive = skill.BuffIsActive;
                        LotDOnCooldown = skill.IsOnCooldown;
                        LotdEquipped = true;
						var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        LotDBuffActive = buff.TimeLeftSeconds[0] > 0.5;
                    }
                    
                    if (skill.SnoPower.Sno == 462089)//Necromancer_SkeletalMage { get; }
                    {
                        SkeleMagesEquipped = true;
                        HashSet<ActorSnoEnum> SkeletonMageActorSNOs = new HashSet<ActorSnoEnum>
                        {
                            ActorSnoEnum._p6_necro_skeletonmage_a, // Skeleton Mage - No Rune
                            ActorSnoEnum._p6_necro_skeletonmage_b, // Skeleton Mage - Gift of Death
                            ActorSnoEnum._p6_necro_skeletonmage_e, // Skeleton Mage - Contamination
                            ActorSnoEnum._p6_necro_skeletonmage_f_archer, // Skeleton Mage - Archer
                            ActorSnoEnum._p6_necro_skeletonmage_c, // Skeleton Mage - Singularity
                           ActorSnoEnum. _p6_necro_skeletonmage_d  // Skeleton Mage - Life Support
                        };
                        var SkeletonMageActors = Hud.Game.Actors.Where(EachActor => SkeletonMageActorSNOs.Contains(EachActor.SnoActor.Sno) && // Find out which are skeleton mages actors
                                        EachActor.SummonerAcdDynamicId == Hud.Game.Me.SummonerId); // Then find out if they are summoned by the player
                        NumberOfSkeleMages = SkeletonMageActors.Count(); // And then count how many are found
                    }
                    
                    if (skill.SnoPower.Sno == 465350)//Necromancer_Simulacrum { get; }
                    {
                        SimOnCooldown = skill.IsOnCooldown;
                        SimEquipped = true;
                        var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        SimBuffActive = buff.TimeLeftSeconds[0] > 0.0;
                    }

                    if (skill.SnoPower.Sno == 462243)//Necromancer_DeathNova { get; } // 462243
                    {
                        BloodNovaOnCooldown = skill.IsOnCooldown;
                        BloodNovaEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 453801)//Necromancer_CommandSkeletons { get; } // 453801
                    {
                        CommandSkeletonsEquipped = true;
                    }
                    //dh
                    if (skill.SnoPower.Sno == 302846)//DemonHunter_Vengeance { get; }
                    {
						VengeanceOnCooldown = skill.IsOnCooldown;
                        VengeanceEquipped = true;
                        var buff = skill.Buff;
                        if ((buff == null) || (buff.IconCounts[0] <= 0)) continue;
                        VengeanceBuffActive = buff.TimeLeftSeconds[0] > 0.5;
                        
                    }
                    if (skill.SnoPower.Sno == 130831)//DemonHunter_RainOfVengeance { get; }
                    {
                        RainOfVengeanceOnCooldown = skill.IsOnCooldown;
                        RainOfVengeanceEquipped = true;
                    }
                    if (skill.SnoPower.Sno == 129212)//DemonHunter_Preparation { get; }
                    {
                        PreparationOnCooldown = skill.IsOnCooldown;
                        PreparationEquipped = true;
                    }
                }

                var LoadingBuff = player.Powers.GetBuff(212032);
                if (!(LoadingBuff == null || !LoadingBuff.Active))
                {
                    CanCast = false;
                }
                var GhostedBuff = player.Powers.GetBuff(224639);
                if (!(GhostedBuff == null || !GhostedBuff.Active))
                {
                    CanCast = false;
                }
                var InvulBuff = player.Powers.GetBuff(439438);
                if (!(InvulBuff == null || !InvulBuff.Active))
                {
                    CanCast = false;
                }
                var UntargetableDuringBuff = player.Powers.GetBuff(30582);
                if (!(UntargetableDuringBuff == null || !UntargetableDuringBuff.Active))
                {
                    CanCast = false;
                }
            }

            int RatsFound = 0;

            IsBuffable[(int)HeroClass.Barbarian] = true;
            IsBuffable[(int)HeroClass.Wizard] = true;
            IsBuffable[(int)HeroClass.Monk] = true;
            IsBuffable[(int)HeroClass.Necromancer] = true;
            IsBuffable[(int)HeroClass.DemonHunter] = true;
            IsBuffable[(int)HeroClass.Crusader] = true;
            foreach (var player in Hud.Game.Players)//others
            {

                if (player.HeroClassDefinition.HeroClass == HeroClass.Barbarian)
                {
                    if (player.IsMe)
                    {
                        ImBarb = true;
                    }
                    else
                    {
                        BarbIngame = true;
                        BarbHasValidActor = player.HasValidActor;
                        var EfficaciousToxin = player.Powers.GetBuff(403461);
                        if (EfficaciousToxin == null || !EfficaciousToxin.Active)
                        {
                            //chargebarb
                            ChargeBarbPosition = player.FloorCoordinate;
                        }
                        else
                        {
                            //zbarb
                        }
                    }
                }
                if (player.HeroClassDefinition.HeroClass == HeroClass.Wizard)
                {
                    if (player.IsMe)
                    {
                        ImWizard = true;
                        IBuff ConventionBuff = player.Powers.GetBuff(430674);
                        if ((ConventionBuff == null) || (ConventionBuff.IconCounts[0] <= 0)) continue;

                        ConventionLight = ConventionBuff.IconCounts[5] != 0;
                        ConventionArcane = ConventionBuff.IconCounts[1] != 0;
                        ConventionCold = ConventionBuff.IconCounts[2] != 0;
                        ConventionFire = ConventionBuff.IconCounts[3] != 0;

                        IBuff BlackholeBuff = player.Powers.GetBuff(243141);
                        if (BlackholeBuff == null) continue;
                        if (BlackholeBuff.IconCounts[5] <= 0) continue;

                        BlackholeBuffActive = (BlackholeBuff.TimeLeftSeconds[5] > 3.5);
                    }
                    else
                    {
                        WizPosition = player.FloorCoordinate;
                        WizardIngame = true;
                    }
                }
                if (player.HeroClassDefinition.HeroClass == HeroClass.Monk)
                {
                    if (player.IsMe)
                    {
                        ImMonk = true;
                    }
                    else
                    {
                        MonkPosition = player.FloorCoordinate;
                        MonkIngame = true;
                    }
                }
                if (player.HeroClassDefinition.HeroClass == HeroClass.Necromancer)
                {
                    var EfficaciousToxin = player.Powers.GetBuff(403461);
                    if (EfficaciousToxin == null || !EfficaciousToxin.Active)
                    {
                        //rat
                        if (player.IsMe)
                        {
                            ImZnec = false;
							ImNecro = true;
                        }
                        else
                        {
                            if (RatsFound == 0)
                            {
                                Rat1Dead = player.IsDead;
                                Rat1Position = player.FloorCoordinate;
                                RatsFound = 1;
                            }
                            else if (RatsFound == 1)
                            {
                                Rat2Dead = player.IsDead;
                                Rat2Position = player.FloorCoordinate;
                                RatsFound = 2;
                            }
                            NecroPosition = player.FloorCoordinate;
                        }
                    }
                    else
                    {
                        //znec
                        if (player.IsMe)
                        {
                            ImZnec = true;
							ImNecro = true;
                        }
                        foreach (var i in _skillOrder)
                        {
                            var skill = player.Powers.SkillSlots[i];
                            if (skill == null || skill.SnoPower.Sno != 465839)
                                continue; //, //Land of the Dead
                            var buff = skill.Buff;
                            if ((buff != null) && !(buff.IconCounts[0] <= 0))
                            {
                                ZnecLandRunningOut = buff.TimeLeftSeconds[0]<1.0d;
                            }
                        }
                    }

                    foreach (var i in _skillOrder)
                    {
                        var skill = player.Powers.SkillSlots[i];
                        if (skill.SnoPower.Sno == 465350)//Necromancer_Simulacrum { get; }
                        {
                            var CurrentSimCD = skill.CalculateCooldown(120);
                            if (CurrentSimCD < SimCD)
                            {
                                SimCD = CurrentSimCD;
                            }
                            break;
                        }

                    }

                    NecrosIngame++;
                }
                if (player.HeroClassDefinition.HeroClass == HeroClass.DemonHunter)
                {
                    if (player.IsMe)
                    {
                        ImDh = true;
                        IBuff NatBuff = player.Powers.GetBuff(434964);
                        if (NatBuff == null) continue;
                        NatBuffActive = (NatBuff.TimeLeftSeconds[1] > 0.5);
                    }
                    else
                    {
                        //
                    }
                }
                if (player.HeroClassDefinition.HeroClass == HeroClass.Crusader)
                {
                    if (player.IsMe)
                    {
                        ImSader = true;
                    }
                    else
                    {
                        //
                    }
                }

                var actors2 = Hud.Game.Actors.Where(x3 => x3.SnoActor.Sno == ActorSnoEnum._generic_proxy && x3.GetAttributeValueAsInt(Hud.Sno.Attributes.Power_Buff_1_Visual_Effect_None, Hud.Sno.SnoPowers.OculusRing.Sno) == 1);
                if (actors2.Count() >= 2)
                {
                    //foreach (var actor2 in actors2)
                    //{
                    //DecoratorBase.Paint(layer, actor, actor.FloorCoordinate, null);
                    //if (actor.FloorCoordinate.XYDistanceTo(MePosition) < 13.3f)
                    //    {
                    //        DecoratorInside.Paint(layer, actor, actor.FloorCoordinate, null); 
                    //    }

                    //}
                    var ocu1 = actors2.Skip(0).First();
                    var ocu2 = actors2.Skip(1).First();
                    var ocu1Coor = ocu1.FloorCoordinate;
                    var ocu1ScreenCoor = ocu1Coor.ToScreenCoordinate();
                    var ocu2Coor = ocu2.FloorCoordinate;
                    var ocu2ScreenCoor = ocu2Coor.ToScreenCoordinate();


                    //Schnittmenge
                    if (ocu1.FloorCoordinate.XYDistanceTo(ocu2.FloorCoordinate) < 26.6f)
                    {
                        OcuX = (ocu1ScreenCoor.X + ocu2ScreenCoor.X) / 2;
                        OcuY = (ocu1ScreenCoor.Y + ocu2ScreenCoor.Y) / 2;
                        OcuX *= scrfaktor;
                        OcuY *= scrfaktor;

                    }
                    else
                    {
                        if (ocu1.FloorCoordinate.XYDistanceTo(Hud.Game.Players.First().FloorCoordinate) < ocu2.FloorCoordinate.XYDistanceTo(Hud.Game.Players.First().FloorCoordinate))
                        {
                            OcuX = ocu1ScreenCoor.X;
                            OcuY = ocu1ScreenCoor.Y;
                            OcuX *= scrfaktor;
                            OcuY *= scrfaktor;
                        }
                        else
                        {
                            OcuX = ocu2ScreenCoor.X;
                            OcuY = ocu2ScreenCoor.Y;
                            OcuX *= scrfaktor;
                            OcuY *= scrfaktor;
                        }

                    }
                }
                else if (actors2.Count() == 1)
                {
                    var ocu3 = actors2.Skip(0).First();
                    var ocu3Coor = ocu3.FloorCoordinate;
                    var ocu3ScreenCoor = ocu3Coor.ToScreenCoordinate();
                    OcuX = ocu3ScreenCoor.X;
                    OcuY = ocu3ScreenCoor.Y;
                    OcuX *= scrfaktor;
                    OcuY *= scrfaktor;
                }




                var shocktower = Hud.Game.Actors.Where(x2 => x2.SnoActor.Sno == ActorSnoEnum._x1_pand_ext_ordnance_tower_shock_a); // 322194

                foreach (var actor in shocktower)
                {
                    if (actor.IsOnScreen)
                    {
                        ShockCoor = actor.FloorCoordinate;
                        shockexist = true;
                    }
                }



                double LowestHealth = 90000000000000000.0;
                foreach (var monster in Hud.Game.AliveMonsters)
                {



                    if (actors2.Count() == 0 && ImNecro && Hud.Game.SpecialArea == SpecialArea.GreaterRift)
                    {
                        if ((monster.CurHealth < LowestHealth) && (monster.IsOnScreen) && (monster.FloorCoordinate.XYDistanceTo(Hud.Game.Me.FloorCoordinate) <= 25))
                        {
                            var MonsterCoor = monster.FloorCoordinate.Offset(0, 0, (monster.RadiusScaled * 3));
                            var ScreenCoor = MonsterCoor.ToScreenCoordinate();

                            PosiX = ScreenCoor.X;
                            PosiY = ScreenCoor.Y;
                            PosiX *= scrfaktor;
                            PosiY *= scrfaktor;
                            LowestHealth = monster.CurHealth;
                        }
                    }
                    else
                    {

                        if ((monster.IsElite) && (monster.IsOnScreen) && (monster.Rarity != ActorRarity.RareMinion) && !monster.Illusion && (!(monster.GetAttributeValue(Hud.Sno.Attributes.Power_Buff_0_Visual_Effect_None, 226438, 0) != 0)))
                        {
                            //if (HasAffix(monster, MonsterAffix.Juggernaut))
                            //{
                            //    var MonsterCoor = monster.FloorCoordinate.Offset(0, 0, (monster.RadiusScaled * 3));
                            //    var ScreenCoor = MonsterCoor.ToScreenCoordinate();

                            //    PosiX = ScreenCoor.X;
                            //    PosiY = ScreenCoor.Y;
                            //    PosiX *= scrfaktor;
                            //    PosiY *= scrfaktor;
                            //    LowestHealth = 1.0;
                            //}
                            if (monster.CurHealth < LowestHealth)
                            {
                                var MonsterCoor = monster.FloorCoordinate.Offset(0, 0, (monster.RadiusScaled * 3));
                                var ScreenCoor = MonsterCoor.ToScreenCoordinate();

                                PosiX = ScreenCoor.X;
                                PosiY = ScreenCoor.Y;
                                PosiX *= scrfaktor;
                                PosiY *= scrfaktor;
                                LowestHealth = monster.CurHealth;
                            }
                        }

                    }
                }

                if (shockexist)
                {
                    var MonsterCoor2 = ShockCoor;
                    var ScreenCoor2 = MonsterCoor2.ToScreenCoordinate();
                    PosiX = ScreenCoor2.X;
                    PosiY = ScreenCoor2.Y;
                    PosiX *= scrfaktor;
                    PosiY *= scrfaktor;
                }


                var inRift2 = Hud.Game.SpecialArea == SpecialArea.Rift || Hud.Game.SpecialArea == SpecialArea.GreaterRift;
                int density = 0;

                int density2 = 0;
                count2 = 0;
                MRiftProgression = 0;
                Mcount = 0;
                elitecount = 0;


                if (ImWizard && Hud.Game.SpecialArea == SpecialArea.GreaterRift)
                {
                    int ecount = 0;

                    foreach (var monster2 in Hud.Game.AliveMonsters)
                    {
                        double MonsterRiftProgression = 0;
                        if (monster2.IsOnScreen)
                        {
                            int count2 = 0;

                            int count = Hud.Game.AliveMonsters.Count(m => (monster2.FloorCoordinate.XYZDistanceTo(m.FloorCoordinate) - m.RadiusBottom) <= Distance);
                            var monsters3 = Hud.Game.AliveMonsters.Where(mm => (monster2.FloorCoordinate.XYZDistanceTo(mm.FloorCoordinate) - mm.RadiusBottom) <= MaxRadius);
                            if (inRift2)
                            {
                                foreach (var monster3 in monsters3)
                                {
                                    if (monster3.IsElite && monster3.Rarity == ActorRarity.Rare && monster3.FloorCoordinate.XYZDistanceTo(monster2.FloorCoordinate) <= 10)
                                    {
                                        ecount += 3;
                                    }
                                    else if (monster3.IsElite && monster3.Rarity == ActorRarity.Champion && monster3.FloorCoordinate.XYZDistanceTo(monster2.FloorCoordinate) <= 10)
                                    {
                                        ecount += 1;
                                    }
                                    else
                                    {
                                        MonsterRiftProgression += monster3.SnoMonster.RiftProgression * 100.0d / this.Hud.Game.MaxQuestProgress;
                                    }
                                }
                            }

                            IWorldCoordinate itsmee = Hud.Game.Players.First().FloorCoordinate;
                            bool itsnearmee = monster2.FloorCoordinate.XYZDistanceTo(itsmee) <= 10;
                            if (count >= density && ecount >= elitecount)
                            {
                                MCoordinate = monster2.FloorCoordinate.Offset(0, 0, (monster2.RadiusScaled));
                                Mcount = count;
                                density = count;
                                elitecount = ecount;
                                MRiftProgression = MonsterRiftProgression;
                            }
                        }

                        if (Mcount > 0)
                        {
                            var MonsterCoor = MCoordinate;
                            var ScreenCoor = MonsterCoor.ToScreenCoordinate();
                            PosiX = ScreenCoor.X;
                            PosiY = ScreenCoor.Y;
                            PosiX *= scrfaktor;
                            PosiY *= scrfaktor;
                        }
                        else
                        {
                            PosiX = 0.0f;
                            PosiY = 0.0f;
                        }
                    }
                }

                if (player.IsMe) continue;
                var LoadingBuff = player.Powers.GetBuff(212032);
                if (!(LoadingBuff == null || !LoadingBuff.Active))
                {
                    IsBuffable[(int)player.HeroClassDefinition.HeroClass] = false;
                }
                var GhostedBuff = player.Powers.GetBuff(224639);
                if (!(GhostedBuff == null || !GhostedBuff.Active))
                {
                    IsBuffable[(int)player.HeroClassDefinition.HeroClass] = false;
                }
                var InvulBuff = player.Powers.GetBuff(439438);
                if (!(InvulBuff == null || !InvulBuff.Active))
                {
                    IsBuffable[(int)player.HeroClassDefinition.HeroClass] = false;
                }
                var UntargetableDuringBuff = player.Powers.GetBuff(30582);
                if (!(UntargetableDuringBuff == null || !UntargetableDuringBuff.Active))
                {
                    IsBuffable[(int)player.HeroClassDefinition.HeroClass] = false;
                }
            }

            PartyIsBuffable = IsBuffable[(int)HeroClass.Barbarian] &&
            IsBuffable[(int)HeroClass.Wizard] &&
            IsBuffable[(int)HeroClass.Monk] &&
            IsBuffable[(int)HeroClass.Necromancer] &&
            IsBuffable[(int)HeroClass.DemonHunter] &&
            IsBuffable[(int)HeroClass.Crusader];

            WizDistance = WizPosition.XYDistanceTo(MyPosition);
            MonkDistance = MonkPosition.XYDistanceTo(MyPosition);
            NecroDistance = NecroPosition.XYDistanceTo(MyPosition);
            ChargeBarbDistance = ChargeBarbPosition.XYDistanceTo(MyPosition);
            Rat1Distance = Rat1Position.XYDistanceTo(MyPosition);
            Rat2Distance = Rat2Position.XYDistanceTo(MyPosition);

            var Channelingbuff = Hud.Game.Me.Powers.GetBuff(266258);
            if ((Channelingbuff != null) && (Channelingbuff.IconCounts[0] > 0))
            {
                CastSimInChanneling = (Channelingbuff.TimeLeftSeconds[0] < 2.0d) || (Channelingbuff.TimeLeftSeconds[0] > 28.0d);
                ChannelingAfterDelay = Channelingbuff.TimeLeftSeconds[0] < 28.0d;
            }

            var TargetedMonster = Hud.Game.SelectedMonster2 ?? Hud.Game.SelectedMonster1;
            bool EliteTargeted = false;
            if (TargetedMonster != null)
            {
                //EliteTargeted = TargetedMonster.IsElite;
                EliteTargeted = (TargetedMonster.Rarity == ActorRarity.Unique) ||
                                (TargetedMonster.Rarity == ActorRarity.Champion) ||
                                (TargetedMonster.Rarity == ActorRarity.Rare) ||
                                (TargetedMonster.SnoMonster.Priority == MonsterPriority.boss) ||
                                (TargetedMonster.SnoActor.Sno == ActorSnoEnum._x1_pand_ext_ordnance_tower_shock_a);//shocktower
            }


            InARift = (Hud.Game.SpecialArea == SpecialArea.None || Hud.Game.SpecialArea == SpecialArea.UberFight || Hud.Game.SpecialArea == SpecialArea.Rift || Hud.Game.SpecialArea == SpecialArea.GreaterRift);


            bool GRGuardianIsDead = false;
            if (riftQuest != null)
            {
                if (Hud.Game.Monsters.Any(m => m.Rarity == ActorRarity.Boss && !m.IsAlive))
                {
                    GRGuardianIsDead = true;
                }
                /*else
                {
                    GRGuardianIsDead = (riftQuest.QuestStepId == 34 || riftQuest.QuestStepId == 46);
                };*/
            }

            Active = Active && !GRGuardianIsDead;


            double Cooldown = (Hud.Game.Me.Powers.HealthPotionSkill.CooldownFinishTick - Hud.Game.CurrentGameTick) / 60d;
            bool PotionIsOnCooldown = Cooldown <= 30 && Cooldown >= 0 ? true : false;

            bool WizInIpRange;
            if (WizardIngame && MonkIngame)//metas
            {
                if ((WizPosition.XYDistanceTo(MonkPosition) > 50) || !MonkIngame)
                {
                    WizInIpRange = (WizDistance <= 45);
                }
                else
                {
                    WizInIpRange = (Math.Max(WizDistance, MonkDistance) <= 45);
                }

                CastIp = CanCast && !IPOnCooldown && IpEquipped && (!BossIsSpawned && WizInIpRange && IsBuffable[(int)HeroClass.Wizard] ||
                           BossIsSpawned && (NecroDistance <= 45) && IsBuffable[(int)HeroClass.Necromancer]);
                CastFalter = CanCast && FalterEquipped && !FalterOnCooldown && (!BossIsSpawned && (WizDistance <= 20) ||
                           BossIsSpawned);
                CastWc = CanCast && WcEquipped && !WarCryOnCooldown && PartyIsBuffable && (!ChilanikBuff || WizDistance <= 100 || BossIsSpawned);
            }
            else if (NecrosIngame == 3)//rats
            {
                CastIp = CanCast && !IPOnCooldown && IpEquipped && PartyIsBuffable;
                if (!Rat1Dead)
                {
                    CastIp = CastIp && (Rat1Distance <= 45);
                }
                if (!Rat2Dead)
                {
                    CastIp = CastIp && (Rat2Distance <= 45);
                }
                CastFalter = CanCast && FalterEquipped && !FalterOnCooldown;
                CastWc = CanCast && WcEquipped && !WarCryOnCooldown && PartyIsBuffable;
            }
            else
            {
                CastIp = CanCast && !IPOnCooldown && IpEquipped && PartyIsBuffable && (Hud.Game.NumberOfPlayersInGame == (Hud.Game.Players.Where(p => p.CentralXyDistanceToMe <= 45)).Count());
                CastFalter = CanCast && FalterEquipped && !FalterOnCooldown;
                CastWc = CanCast && (WcEquipped && !WarCryOnCooldown && PartyIsBuffable && (!ChilanikBuff || (Hud.Game.NumberOfPlayersInGame == (Hud.Game.Players.Where(p => p.CentralXyDistanceToMe <= 45)).Count())));
            }

	    var MyPosi = Hud.Game.Me.FloorCoordinate;
            var MyScreenCoor = MyPosi.ToScreenCoordinate();
            MyX = MyScreenCoor.X;
            MyY = MyScreenCoor.Y;
            MyX *= scrfaktor;
            MyY *= scrfaktor;
            
            


            CastBerserker = CanCast && BerserkerEquipped && !BerserkerOnCooldown && !BerserkerBuffActive;
            CastSprint = CanCast && SprintEquipped && !SprintOnCooldown && Hud.Game.Me.Stats.ResourceCurFury >= 20 && !SprintBuffActive;
            CastEpiphany = CanCast && EpiphanyEquipped && !EpiphanyOnCooldown && !EpiphanyBuffActive;
            CastMantraHealing = CanCast && MantraHealingEquipped && !MantraOfHealingOnCooldown && Hud.Game.Me.Stats.ResourceCurSpirit >= (0.3 * Hud.Game.Me.Stats.ResourceMaxSpirit) && (Range75Enemies >= 1);
            CastSweepingWind = CanCast && SweepingWindEquipped && RecastSweepingWind && Hud.Game.Me.Stats.ResourceCurSpirit >= (Hud.Game.Me.Stats.ResourceMaxSpirit / 2.0);
            CastBoh = CanCast && BohEquipped && !BohOnCooldown && ((Hud.Game.SpecialArea == SpecialArea.Rift) || BarbIngame && (ChargeBarbDistance <= 12) && BarbHasValidActor);
            CastMantraConviction = CanCast && MantraConvictionEquipped && Hud.Game.Me.Stats.ResourceCurSpirit >= (Hud.Game.Me.Stats.ResourceMaxSpirit / 2.0);
            CastLotd = CanCast && LotdEquipped && !LotDOnCooldown && (!LotDBuffActive || CastSimInChanneling) && (Range75Enemies >= 1);// && SimOnCooldown;
            //CastBoneArmor = BoneArmorEquipped && (((BoneArmorAlmostRunningOut && (Range25Enemies >= 1)) ||
            //                  (!BoneArmorAlmostRunningOut && (Range25Enemies >= 5))) &&
            //                  !BoneArmorOnCooldown);
            //CastBoneArmor = CanCast && BoneArmorEquipped && (Range25Enemies >= 1) && !BoneArmorOnCooldown;

            CastBoneArmor = BoneArmorEquipped && (((BoneArmorAlmostRunningOut && (Range25Enemies >= 1)) ||
                              (!BoneArmorAlmostRunningOut && (Range25Enemies >= 5))) &&
                              !BoneArmorOnCooldown);

            DontCastSim = (riftTimer.ElapsedMilliseconds < (1000.0*SimCD*0.4)) || ChannelingAfterDelay;
			if(Hud.Game.SpecialArea == SpecialArea.Rift) DontCastSim = false;

            if (riftTimer.ElapsedMilliseconds < 10000.0)//first 10s
            {
                DontCastLand = true;
            }
            else if (riftTimer.ElapsedMilliseconds < 20000.0)//10-20s
            {
                DontCastLand = !ZnecLandRunningOut;
            }
            else if (riftTimer.ElapsedMilliseconds < 30000.0)//20-30s
            {
                DontCastLand = false;
            }
            else if (riftTimer.ElapsedMilliseconds < (1000.0 * SimCD * 0.4 + 30000.0 + 1000.0))//wait till first sim runs out
            {
                DontCastLand = true;
            }
            else
            {
                DontCastLand = !(Hud.Game.Me.Stats.ResourceMaxEssence > 450);//have sim because of reservoir;
            }
            if (Hud.Game.SpecialArea == SpecialArea.Rift) DontCastLand = false;

            CastSim = CanCast && InARift && SimEquipped && !SimOnCooldown && (!(Hud.Game.Me.Stats.ResourceMaxEssence > 450) || CastSimInChanneling);//have sim because of reservoir
            CastPotion = CanCast && Hud.Game.Me.Defense.HealthCur <= (Hud.Game.Me.Defense.HealthMax * 0.75) && !PotionIsOnCooldown;
            CastStormArmor = CanCast && !ArchonBuffActive && StormArmorEquipped && !StormArmorOnCooldown && !StormArmorBuffActive;
            CastMagicWeapon = CanCast && !ArchonBuffActive && MagicWeaponEquipped && !MagicWeaponOnCooldown && !MagicWeaponBuffActive;
            CastVengeance = CanCast && VengeanceEquipped && !VengeanceOnCooldown && !VengeanceBuffActive;
            CastRainOfVengeance = CanCast && RainOfVengeanceEquipped && !RainOfVengeanceOnCooldown && !NatBuffActive;
            CastPreparation = CanCast && PreparationEquipped && !PreparationOnCooldown && !(Hud.Game.Me.Stats.ResourceCurDiscipline >= (Hud.Game.Me.Stats.ResourceMaxDiscipline - 30));
            ForceMove = CanCast && !ImZnec && (Hud.Game.Me.AnimationState == AcdAnimationState.Idle || Hud.Game.Me.AnimationState == AcdAnimationState.Casting);
            CastArcaneBlast = CanCast && ArchonBuffActive && ArchonEquipped && !ArcaneBlastOnCooldown && (Range15Enemies > 0);
            CastExplosiveBlast = CanCast && !ArchonBuffActive && ExplosiveBlastEquippped && !ExplosiveBlastOnCooldown && Hud.Game.Me.Stats.ResourceCurArcane >= (Hud.Game.Me.Stats.ResourceMaxArcane / 3.0);
            CastBloodNova = CanCast && BloodNovaEquipped && !BloodNovaOnCooldown && (Hud.Game.Me.Stats.ResourceCurEssence >= (Hud.Game.Me.Stats.ResourceMaxEssence / 3.0)) && (Range25Enemies >= 1);
            CastBlindingFlash = CanCast && BlindingFlashEquipped && !BlindingFlashActive && !BlindingFlashOnCooldown;
            CastCommandSkeletons = CanCast && CommandSkeletonsEquipped;

            if (BossIsSpawned || Hud.Game.SpecialArea == SpecialArea.Rift)
			{
				CastSkeleMages = CanCast && SkeleMagesEquipped && (Hud.Game.Me.Stats.ResourceCurEssence >= (Hud.Game.Me.Stats.ResourceMaxEssence*0.95)) && (NumberOfSkeleMages < 10);
            }
			else
			{
				CastSkeleMages = CanCast && SkeleMagesEquipped && (Hud.Game.Me.Stats.ResourceCurEssence >= (Hud.Game.Me.Stats.ResourceMaxEssence*0.95)) && (EliteInRange || (!EliteInRange) && (NumberOfSkeleMages < 10));
			}
			

            Bytes[0] = Set(Bytes[0], 0, true);
            Bytes[0] = Set(Bytes[0], 1, Active);
            Bytes[0] = Set(Bytes[0], 2, ImBarb);
            Bytes[0] = Set(Bytes[0], 3, ImMonk);
            Bytes[0] = Set(Bytes[0], 4, ImWizard);
            Bytes[0] = Set(Bytes[0], 5, ImNecro);
            Bytes[0] = Set(Bytes[0], 6, ImDh);
            Bytes[0] = Set(Bytes[0], 7, ImSader);

            Bytes[1] = Set(Bytes[1], 0, true);
            Bytes[1] = Set(Bytes[1], 1, ConventionLight);
            Bytes[1] = Set(Bytes[1], 2, ConventionArcane);
            Bytes[1] = Set(Bytes[1], 3, ConventionCold);
            Bytes[1] = Set(Bytes[1], 4, ConventionFire);
            Bytes[1] = Set(Bytes[1], 5, BlackholeBuffActive);
            Bytes[1] = Set(Bytes[1], 6, CastArcaneBlast);
            Bytes[1] = Set(Bytes[1], 7, false);

            Bytes[2] = Set(Bytes[2], 0, true);
            Bytes[2] = Set(Bytes[2], 1, InARift);
            Bytes[2] = Set(Bytes[2], 2, DontCastLand);
            Bytes[2] = Set(Bytes[2], 3, CastBlindingFlash);
            Bytes[2] = Set(Bytes[2], 4, CastCommandSkeletons);
            Bytes[2] = Set(Bytes[2], 5, false);
            Bytes[2] = Set(Bytes[2], 6, false);
            Bytes[2] = Set(Bytes[2], 7, false);

            Bytes[3] = Set(Bytes[3], 0, true);
            Bytes[3] = Set(Bytes[3], 1, CastIp);
            Bytes[3] = Set(Bytes[3], 2, CastSim);
            Bytes[3] = Set(Bytes[3], 3, DontCastSim);
            Bytes[3] = Set(Bytes[3], 4, CastFalter);
            Bytes[3] = Set(Bytes[3], 5, CastBerserker);
            Bytes[3] = Set(Bytes[3], 6, CastSprint);
            Bytes[3] = Set(Bytes[3], 7, CastEpiphany);

            Bytes[4] = Set(Bytes[4], 0, true);
            Bytes[4] = Set(Bytes[4], 1, CastWc); 
            Bytes[4] = Set(Bytes[4], 2, CastMantraHealing);
            Bytes[4] = Set(Bytes[4], 3, CastSweepingWind);
            Bytes[4] = Set(Bytes[4], 4, CastBoh);
            Bytes[4] = Set(Bytes[4], 5, CastMantraConviction);
            Bytes[4] = Set(Bytes[4], 6, CastLotd);
            Bytes[4] = Set(Bytes[4], 7, CastBoneArmor);

            Bytes[5] = Set(Bytes[5], 0, true);
            Bytes[5] = Set(Bytes[5], 1, CastPotion);
            Bytes[5] = Set(Bytes[5], 2, CastStormArmor);
            Bytes[5] = Set(Bytes[5], 3, CastMagicWeapon);
            Bytes[5] = Set(Bytes[5], 4, CastVengeance);
            Bytes[5] = Set(Bytes[5], 5, CastRainOfVengeance);
            Bytes[5] = Set(Bytes[5], 6, CastPreparation);
            Bytes[5] = Set(Bytes[5], 7, CastSkeleMages);

            Bytes[6] = Set(Bytes[6], 0, true);
            Bytes[6] = Set(Bytes[6], 1, ForceMove);
            Bytes[6] = Set(Bytes[6], 2, CastExplosiveBlast);
            Bytes[6] = Set(Bytes[6], 3, CastBloodNova);

            Bytes[7] = Set(Bytes[7], 0, true);

            GetByteFromFloat(PosiX, ref BitX[0], ref BitX[1], ref BitX[2], ref BitX[3]);
            GetByteFromFloat(PosiY, ref BitY[0], ref BitY[1], ref BitY[2], ref BitY[3]);

            GetByteFromFloat(DBX, ref DBBitX[0], ref DBBitX[1], ref DBBitX[2], ref DBBitX[3]);
            GetByteFromFloat(DBY, ref DBBitY[0], ref DBBitY[1], ref DBBitY[2], ref DBBitY[3]);

            GetByteFromFloat(OcuX, ref OcuBitX[0], ref OcuBitX[1], ref OcuBitX[2], ref OcuBitX[3]);
            GetByteFromFloat(OcuY, ref OcuBitY[0], ref OcuBitY[1], ref OcuBitY[2], ref OcuBitY[3]);

            GetByteFromFloat((float)Latenz, ref LatenzBit[0], ref LatenzBit[1], ref LatenzBit[2], ref LatenzBit[3]);

            GetByteFromFloat(MyX, ref MyBitX[0], ref MyBitX[1], ref MyBitX[2], ref MyBitX[3]);
            GetByteFromFloat(MyY, ref MyBitY[0], ref MyBitY[1], ref MyBitY[2], ref MyBitY[3]);


            byte[] dataout = Bytes.Concat(BitX).Concat(BitY).Concat(DBBitX).Concat(DBBitY).Concat(OcuBitX).Concat(OcuBitY).Concat(LatenzBit).Concat(MyBitX).Concat(MyBitY).ToArray();

            if (Enabled) stream.Write(dataout, 0, dataout.Length);
            System.Threading.Thread.Sleep(2);
        }

    }

}