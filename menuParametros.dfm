object Form2: TForm2
  Left = 332
  Top = 264
  Width = 504
  Height = 200
  BorderWidth = 1
  Caption = 'Menu de Opcoes de Parametros'
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
    Left = 136
    Top = 32
    Width = 113
    Height = 24
    Caption = 'Erro m'#225'ximo:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 24
    Top = 88
    Width = 281
    Height = 24
    Caption = 'Aproxima'#231#227'o inicial das vari'#225'veis:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 0
    Top = 128
    Width = 4
    Height = 20
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object erroMaxMenu: TEdit
    Left = 352
    Top = 32
    Width = 105
    Height = 32
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    Text = '1e-6'
    OnChange = erroMaxMenuChange
    OnClick = erroMaxMenuChange
  end
  object aproxInicialCorrentes: TEdit
    Left = 352
    Top = 88
    Width = 105
    Height = 32
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    Text = '0.1'
    OnChange = aproxInicialCorrentesChange
    OnClick = aproxInicialCorrentesChange
  end
end
