object Form3: TForm3
  Left = 342
  Top = 214
  Width = 416
  Height = 339
  Caption = 'Op'#231#245'es AC'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 24
    Top = 40
    Width = 159
    Height = 24
    Caption = 'N'#250'mero de Pontos'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 24
    Top = 104
    Width = 149
    Height = 24
    Caption = 'Frequ'#234'ncia Inicial'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 24
    Top = 160
    Width = 143
    Height = 24
    Caption = 'Frequ'#234'ncia Final'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 24
    Top = 224
    Width = 123
    Height = 24
    Caption = 'Tipo da escala'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object numPontosMenu: TEdit
    Left = 264
    Top = 40
    Width = 105
    Height = 32
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    Text = '1000'
    OnChange = numPontosMenu1
    OnClick = numPontosMenu1
  end
  object freInicialMenu: TEdit
    Left = 264
    Top = 96
    Width = 105
    Height = 32
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    Text = '1'
    OnChange = freInicialMenu1
    OnClick = freInicialMenu1
  end
  object freFinalMenu: TEdit
    Left = 264
    Top = 152
    Width = 105
    Height = 32
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    Text = '10000'
    OnChange = freFinalMenu1
    OnClick = freFinalMenu1
  end
  object ComboBox1: TComboBox
    Left = 256
    Top = 216
    Width = 129
    Height = 32
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ItemHeight = 24
    ItemIndex = 0
    ParentFont = False
    TabOrder = 3
    Text = 'LIN'
    OnChange = ComboBox11
    OnClick = ComboBox11
    Items.Strings = (
      'LIN'
      'DEC'
      'OCT')
  end
end
